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

#include <libudev.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>

struct CameraInfo {
    std::string usb_port;
    std::string serial;
    std::string uid;
    std::vector<std::string> dev_nodes;
};

int main() {
    struct udev* udev = udev_new();
    if (!udev) {
        std::cerr << "Failed to initialize udev" << std::endl;
        return 1;
    }

    struct udev_enumerate* enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "video4linux");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    std::map<std::string, CameraInfo> cameras;

    udev_list_entry* dev_list_entry;
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char* path = udev_list_entry_get_name(dev_list_entry);
        struct udev_device* dev = udev_device_new_from_syspath(udev, path);

        const char* devnode = udev_device_get_devnode(dev);
        if (!devnode) {
            udev_device_unref(dev);
            continue;
        }

        // 找 USB 父设备
        struct udev_device* usb_dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        if (usb_dev) {
            const char* vid = udev_device_get_sysattr_value(usb_dev, "idVendor");
            const char* pid = udev_device_get_sysattr_value(usb_dev, "idProduct");
            const char* serial = udev_device_get_sysattr_value(usb_dev, "serial");
            const char* devpath = udev_device_get_devpath(usb_dev); // USB 端口信息

            std::string key = serial ? serial : (std::string(vid) + ":" + std::string(pid));
            CameraInfo& cam = cameras[key];
            cam.usb_port = devpath ? devpath : "";
            cam.serial = serial ? serial : "N/A";
            cam.uid = std::string(vid) + ":" + std::string(pid) + "_" + cam.serial;
            cam.dev_nodes.push_back(devnode);
        }
        udev_device_unref(dev);
    }

    for (const auto& kv : cameras) {
        const CameraInfo& cam = kv.second;
        std::cout << "==============================" << std::endl;
        std::cout << "USB Port : " << cam.usb_port << std::endl;
        std::cout << "Serial   : " << cam.serial << std::endl;
        std::cout << "UID      : " << cam.uid << std::endl;
        std::cout << "DevNodes : ";
        for (const auto& node : cam.dev_nodes) {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    return 0;
}
