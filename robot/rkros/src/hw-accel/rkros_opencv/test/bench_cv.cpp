/*
 * Copyright (C) 2025 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <optional>
#include <sched.h>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "rkros_opencv/rkcv.hpp"

#define USE_CV_UMAT 1

#if USE_CV_UMAT
typedef cv::UMat CV_MAT;
#else
typedef cv::Mat CV_MAT;
#endif

struct TestCase {
    std::string name;
    CV_MAT mat;
    std::function<void(const CV_MAT &src, CV_MAT &dst)> func = nullptr;
};

/* ---------- 参数 ---------- */
typedef struct {
    std::string image_path;
    int warmup = 1;
    int runs = 200;
    int opencl = 1;
    enum class Allocator { Std, DmaBuf } allocator = Allocator::Std;
} Config;

static std::optional<Config> g_cfg;

static double TRIM = 0.1; // 去掉头尾 10 %
static const int WIDTH = 1920;
static const int HEIGHT = 1080;
static const int TYPE = CV_8UC3;

/* -------------------------- */

/* ======== 在这里继续加你想测的算法 ======== */
std::vector<TestCase> func_list = {

    {"Resize x0.5", CV_MAT(HEIGHT / 2, WIDTH / 2, TYPE),
     [](const CV_MAT &src, CV_MAT &dst) {
         cv::resize(src, dst, {}, 0.5, 0.5, cv::INTER_LINEAR);
     }},

    {"copyTo", CV_MAT(HEIGHT, WIDTH, TYPE),
     [](const CV_MAT &src, CV_MAT &dst) { cv::copyTo(src, dst, cv::noArray()); }},

    {"crop(512x512)", CV_MAT(512, 512, TYPE),
     [](const CV_MAT &src, CV_MAT &dst) {
         src(cv::Rect{100, 100, 512, 512}).copyTo(dst);
     }},

    {"flip(horizontally)", CV_MAT(HEIGHT, WIDTH, TYPE),
     [](const CV_MAT &src, CV_MAT &dst) {
         cv::flip(src, dst, 1); // 1 表示水平翻转
     }},

    {"rotate(90° clockwise)", CV_MAT(WIDTH, HEIGHT, TYPE),
     [](const CV_MAT &src, CV_MAT &dst) {
         cv::rotate(src, dst, cv::ROTATE_90_CLOCKWISE);
     }},

    {"cvtColor(bgr -> rgb)", CV_MAT(HEIGHT, WIDTH, TYPE),
     [](const CV_MAT &src, CV_MAT &dst) { cv::cvtColor(src, dst, cv::COLOR_BGR2RGB); }},
};
/* ========================================= */

static double tick() {
    using namespace std::chrono;
    static const auto ori = high_resolution_clock::now();
    return duration<double, std::micro>(high_resolution_clock::now() - ori).count();
}

static double getCurrentCpuUsage() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    std::string cpu;
    iss >> cpu;
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal;
    iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    unsigned long total = user + nice + system + idle + iowait + irq + softirq + steal;
    unsigned long idle_time = idle + iowait;
    static unsigned long last_total = 0, last_idle = 0;
    double usage = 0.0;
    if (last_total != 0) {
        unsigned long total_diff = total - last_total;
        unsigned long idle_diff = idle_time - last_idle;
        usage = 100.0 * (1.0 - (double)idle_diff / total_diff);
    }
    last_total = total;
    last_idle = idle_time;
    return usage;
}

enum WarmUpType { WARMUP_READ, WARMUP_WRITE, WARMUP_RNG, WARMUP_NONE };

void warmup(CV_MAT m, WarmUpType wtype) {
    switch (wtype) {
    case WARMUP_READ:
        cv::sum(m.reshape(1));
        return;
    case WARMUP_WRITE:
        m.reshape(1).setTo(cv::Scalar::all(0));
        return;
    default:
        return;
    }
}

static double bench_once(const TestCase &tc, const CV_MAT &src, CV_MAT &dst) {
    double t0 = tick();
    tc.func(src, dst);
#if USE_CV_UMAT
    cv::ocl::finish(); // 只确保GPU处理完成
#endif
    double t1 = tick();

    return t1 - t0;
}

void fillGradient(cv::Mat &img, int delta = 5) {
    const int ch = img.channels();
    CV_Assert(!img.empty() && img.depth() == CV_8U && ch <= 4);

    int n = 255 / delta;
    int r, c, i;
    for (r = 0; r < img.rows; r++) {
        int kR = r % (2 * n);
        int valR = (kR <= n) ? delta * kR : delta * (2 * n - kR);
        for (c = 0; c < img.cols; c++) {
            int kC = c % (2 * n);
            int valC = (kC <= n) ? delta * kC : delta * (2 * n - kC);
            uchar vals[] = {uchar(valR), uchar(valC), uchar(200 * r / img.rows),
                            uchar(255)};
            uchar *p = img.ptr(r, c);
            for (i = 0; i < ch; i++)
                p[i] = vals[i];
        }
    }
}

static std::vector<double> bench_series(TestCase &tc, const CV_MAT &src) {
    // 正式计时
    std::vector<double> costs;
    costs.reserve(g_cfg->runs);
    for (int i = 0; i < g_cfg->runs; ++i) {
        costs.push_back(bench_once(tc, src, tc.mat));
    }
    return costs;
}

static void print_header() {
    std::cout << "|" << std::setw(20) << " Algorithm "
              << " |" << std::setw(10) << " min(us) "
              << " |" << std::setw(10) << " max(us) "
              << " |" << std::setw(10) << " mean(us) "
              << " |" << std::setw(10) << " median(us) "
              << " |" << std::setw(10) << " Load(%) "
              << " |" << std::setw(10) << " 120FPS(%) "
              << " |\n";
    std::cout << "|:" << std::setfill('-') << std::setw(20) << ""
              << ":|" << std::setw(10) << ""
              << ":|" << std::setw(10) << ""
              << ":|" << std::setw(10) << ""
              << ":|" << std::setw(10) << ""
              << ":|" << std::setw(10) << ""
              << ":|" << std::setw(10) << ""
              << ":|\n"
              << std::setfill(' ');
}

static void print_row(const std::string &name, const std::vector<double> &v,
                      const double cpu_usage = 0.0) {
    int mn = v.front();
    int mx = v.back();
    int avg = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    int med = v[v.size() / 2];
    double fps120 = ((double)avg * 120 / 1000000) * cpu_usage;
    std::cout << "|" << std::setw(20) << name << " |" << std::setw(10) << std::fixed
              << std::setprecision(3) << mn << " |" << std::setw(10) << mx << " |"
              << std::setw(10) << avg << " |" << std::setw(10) << med << " |"
              << std::setw(10) << cpu_usage << " |" << std::setw(10) << fps120
              << " |\n";
    ;
}

void bind_to_cpus(int begin, int end) {
    std::printf("bind to cpu %d-%d\n", begin, end);

    cpu_set_t set;
    CPU_ZERO(&set);
    for (int i = begin; i <= end; ++i)
        CPU_SET(i, &set);
    if (sched_setaffinity(0, sizeof(set), &set) == -1) {
        std::cerr << "sched_setaffinity failed\n";
    }
}

// 把 str 转成 int，失败返回 std::nullopt
static std::optional<int> to_int(const std::string &str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::nullopt;
    }
}

// 把 "std|dmabuf" 转成 enum，失败返回 std::nullopt
static std::optional<Config::Allocator> to_allocator(const std::string &s) {
    std::string low = s;
    std::transform(low.begin(), low.end(), low.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (low == "std")
        return Config::Allocator::Std;
    if (low == "dmabuf")
        return Config::Allocator::DmaBuf;
    return std::nullopt;
}

// 打印用法
static void usage(const char *prog) {
    std::cerr
        << "Usage: " << prog
        << " [file=image] [warmup=0|1] [runs=N] [allocator=std|dmabuf] [opencl=0|1]\n";
}

// 解析 argc/argv 到 Config
static std::optional<Config> parse_args(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return std::nullopt;
    }

    Config g_cfg;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto pos = arg.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Unknown argument: " << arg << '\n';
            return std::nullopt;
        }
        std::string key = arg.substr(0, pos);
        std::string val = arg.substr(pos + 1);

        if (key == "warmup") {
            if (auto v = to_int(val))
                g_cfg.warmup = *v;
            else {
                std::cerr << "Invalid warmup value\n";
                return std::nullopt;
            }
        } else if (key == "runs") {
            if (auto v = to_int(val))
                g_cfg.runs = *v;
            else {
                std::cerr << "Invalid runs value\n";
                return std::nullopt;
            }
        } else if (key == "allocator") {
            if (auto v = to_allocator(val))
                g_cfg.allocator = *v;
            else {
                std::cerr << "Invalid allocator value\n";
                return std::nullopt;
            }
        } else if (key == "file") {
            g_cfg.image_path = val;
        } else if (key == "opencl") {
            if (auto v = to_int(val))
                g_cfg.opencl = *v;
            else {
                std::cerr << "Invalid opencl value\n";
                return std::nullopt;
            }
        } else {
            std::cerr << "Unknown key: " << key << '\n';
            return std::nullopt;
        }
    }
    return g_cfg;
}

int main(int argc, char *argv[]) {
    bool use_dmabuf = false;
    g_cfg = parse_args(argc, argv);
    if (!g_cfg)
        return 1;

    std::cout << "file      : " << g_cfg->image_path << '\n'
              << "warmup    : " << g_cfg->warmup << '\n'
              << "runs      : " << g_cfg->runs << '\n'
              << "allocator : "
              << (g_cfg->allocator == Config::Allocator::Std ? "std" : "dmabuf") << '\n'
              << "opencl    : " << g_cfg->opencl << '\n'
              << '\n';

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image> [warmup=1] [runs=200]\n";
        return -1;
    }

    if (g_cfg->opencl) {
        cv::ocl::setUseOpenCL(true);
        if (cv::ocl::haveOpenCL()) {
            cv::ocl::Device device = cv::ocl::Context::getDefault().device(0);
            std::cout << "OpenCL is enabled. Using OpenCL device: " << device.name()
                      << "\n";
        } else {
            std::cout << "OpenCL is not available. Falling back to CPU.\n";
            cv::ocl::setUseOpenCL(false);
        }
    } else {
        cv::ocl::setUseOpenCL(false);
        std::cout << "OpenCL is disabled. Using CPU only.\n";
    }

    if (g_cfg->allocator == Config::Allocator::Std) {
        use_dmabuf = false;
    } else {
        use_dmabuf = true;
    }

    bind_to_cpus(6, 6); // bind cpu

    CV_MAT src;
    if (!g_cfg->image_path.empty()) {
        cv::Mat img = cv::imread(g_cfg->image_path);
        if (img.empty()) {
            std::cerr << "Cannot read " << g_cfg->image_path << "\n";
            return -1;
        }
        img.convertTo(img, TYPE);
        cv::resize(img, img, cv::Size(WIDTH, HEIGHT));
#if USE_CV_UMAT
        if (use_dmabuf) {
            src = rkcv::createUMat(img.rows, img.cols, img.type());
            img.copyTo(src);
        } else {
            src = img.getUMat(cv::ACCESS_READ);
        }
#else
        src = img;
#endif
    } else {
        std::cout << "make gradient image" << std::endl;
#if USE_CV_UMAT
        cv::Mat img(cv::Size(WIDTH, HEIGHT), TYPE);
        fillGradient(img);
        if (use_dmabuf) {
            src = rkcv::createUMat();
            img.copyTo(src);
        } else {
            src = img.getUMat(cv::ACCESS_READ);
        }
#else
        src.create(HEIGHT, WIDTH, TYPE);
        fillGradient(src);
#endif
    }

    int cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    print_header();
    for (auto &tc : func_list) {
#if USE_CV_UMAT
        if (use_dmabuf) {
            tc.mat = rkcv::createUMat(tc.mat.rows, tc.mat.cols, tc.mat.type());
        }
#endif
        if (g_cfg->warmup) {
            // 预热
            warmup(src, WARMUP_READ);
            warmup(tc.mat, WARMUP_WRITE);
        }

        getCurrentCpuUsage();
        auto v = bench_series(tc, src);
        double cpu_usage = getCurrentCpuUsage() * cpu_count;

        std::sort(v.begin(), v.end());
        size_t skip = static_cast<size_t>(g_cfg->runs * TRIM);
        auto costs_trimmed = std::vector<double>(v.begin() + skip, v.end() - skip);

        print_row(tc.name, costs_trimmed, cpu_usage);
    }

    return 0;
}
