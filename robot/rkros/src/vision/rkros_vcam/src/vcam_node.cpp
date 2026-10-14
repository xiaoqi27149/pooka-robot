#include <chrono>
#include <cmath>
#include <cstring>
#include <sstream>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "rkros_vcam/image_generator.hpp"
#include "rkros_vcam/vcam.hpp"
#include "sensor_msgs/msg/image.hpp"

using namespace std::chrono_literals;

namespace rkros_vcam {

VCamNode::VCamNode(const rclcpp::NodeOptions &options) : Node("rkros_vcam", options) {
    // parameters
    publish_fps_ = this->declare_parameter<double>("publish_fps", 30.0);
    gen_fps_ = this->declare_parameter<double>("gen_fps", 30.0);
    width_ = this->declare_parameter<int>("width", 640);
    height_ = this->declare_parameter<int>("height", 480);

    // topics param: list of maps encoded as strings, simple format:
    // topic:format:msgtype e.g. "/camera/rgb:rgb:sensor"
    std::vector<std::string> topics = this->declare_parameter<std::vector<std::string>>(
        "topics", {"/rkros/vcam/image_raw:rgb:sensor"});

    for (auto &t : topics) {
        // parse topic
        TopicConfig cfg;
        std::stringstream ss(t);
        std::getline(ss, cfg.topic_name, ':');
        std::getline(ss, cfg.format, ':');
        std::getline(ss, cfg.msg_type, ':');
        if (cfg.format.empty())
            cfg.format = "rgb";
        if (cfg.msg_type.empty())
            cfg.msg_type = "sensor";

        // create publisher
        PubHolder ph;
        ph.cfg = cfg;
        auto qos = rclcpp::QoS(10);
        if (cfg.msg_type == "sensor") {
            ph.sensor_pub =
                this->create_publisher<sensor_msgs::msg::Image>(cfg.topic_name, qos);
        } else {
            ph.dmabuf_pub =
                this->create_publisher<rkros_dmabuf::ImageMsg>(cfg.topic_name, qos);
        }
        pubs_.push_back(ph);

        // create and start image generators per format
        if (generators_.find(cfg.format) == generators_.end()) {
            RCLCPP_INFO(this->get_logger(), "create %s image generator",
                        cfg.format.c_str());
            auto gen = std::make_shared<rkros_vcam::ImageGenerator>(
                width_, height_, gen_fps_, cfg.format);
            gen->start();
            generators_.emplace(cfg.format, gen);
        }
    }

    RCLCPP_INFO(this->get_logger(),
                "vcam started, gen_fps=%.2f publish_fps=%.2f size=%dx%d topics=%zu",
                gen_fps_, publish_fps_, width_, height_, pubs_.size());

    // start publisher thread
    running_ = true;
    publish_thread_ = std::thread([this]() {
        using clock = std::chrono::steady_clock;
        auto interval_d =
            std::chrono::duration<double>(1.0 / std::max(1e-6, publish_fps_));
        auto interval = std::chrono::duration_cast<clock::duration>(interval_d);
        auto next_time = clock::now();
        while (running_) {
            // publish latest frames from generators_
            publishFrame();
            next_time += interval;
            std::this_thread::sleep_until(next_time);
        }
    });
}

VCamNode::~VCamNode() {
    // stop publisher
    running_ = false;
    if (publish_thread_.joinable())
        publish_thread_.join();

    for (auto &kv : generators_) {
        if (kv.second)
            kv.second->stop();
    }
}

void VCamNode::publishFrame() {
    // for each topic, fetch frame from generator for its format and publish
    for (auto &ph : pubs_) {
        auto it = generators_.find(ph.cfg.format);
        if (it == generators_.end())
            continue;
        rkros_vcam::ImageGenerator::Frame f;
        if (!it->second->get_latest_frame(f))
            continue;

        if (ph.cfg.msg_type == "sensor") {
            auto img = std::make_unique<sensor_msgs::msg::Image>();
            img->header.stamp = this->now();
            img->header.frame_id = "vcam";
            img->height = f.img.height;
            img->width = f.img.width;
            img->encoding = f.encoding;
            img->is_bigendian = 0;
            img->step = f.img.stride;
            img->data.assign(static_cast<uint8_t *>(f.img.dmabuf->addr()),
                             static_cast<uint8_t *>(f.img.dmabuf->addr()) +
                                 f.img.dmabuf->size());
            RCLCPP_INFO(this->get_logger(), "publish sensor %ld", f.frame_no);
            ph.sensor_pub->publish(std::move(*img));
        } else {
            rkros_dmabuf::ImageMsg msg;
            static_cast<rkros_dmabuf::Image &>(msg) = f.img;
            msg.frame_no = f.frame_no;
            msg.timestamp = this->now();
            RCLCPP_INFO(this->get_logger(), "publish dmabuf %ld", f.frame_no);
            ph.dmabuf_pub->publish(msg);
        }
    }
}

} // namespace rkros_vcam

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(rkros_vcam::VCamNode)
