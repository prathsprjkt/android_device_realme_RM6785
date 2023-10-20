//
// Copyright (C) 2022-2023 The LineageOS Project
//
// SPDX-License-Identifier: Apache-2.0
//

#include <cstdlib>
#include <fstream>
#include <cstring>
#include <sys/sysinfo.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <vector>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

constexpr const char* kProcOperatorPath = "/proc/oppoVersion/operatorName";

struct DeviceProps {
    std::string model;
    std::string device;
};

void property_override(const char* prop, const char* value, bool add = true) {
    prop_info* pi = (prop_info*)__system_property_find(prop);

    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

void set_ro_build_prop(const std::string& prop, const std::string& value,
                       bool product = true) {
    std::string prop_name;
    std::vector<std::string> prop_types = {
            "",        "bootimage.",  "odm.",    "odm_dlkm.",   "product.",
            "system.", "system_ext.", "vendor.", "vendor_dlkm."};

    for (const auto& source : prop_types) {
        if (product) {
            prop_name = "ro.product." + source + prop;
        } else {
            prop_name = "ro." + source + "build." + prop;
        }

        property_override(prop_name.c_str(), value.c_str(), false);
    }
}

DeviceProps get_device() {
    std::string device, model, operator_content;
    if (!android::base::ReadFileToString(kProcOperatorPath,
                                         &operator_content)) {
        LOG(ERROR) << "Failed to read file: " << kProcOperatorPath;
        return {"", ""};
    }

    int operator_code = stoi(operator_content);

    DeviceProps device_props;
    switch (operator_code) {
        // realme 6
        case 101:
        case 102:
        case 104:
        case 105:
            device_props = {"RMX2001", "RMX2001L1"};
            break;
        case 106:
            device_props = {"RMX2003", "RMX2003L1"};
            break;
        case 113:
            device_props = {"RMX2005", "RMX2005L1"};
            break;
        // realme 6i/6s/Narzo
        case 111:
        case 112:
        case 114:
            device_props = {"RMX2002", "RMX2002L1"};
            break;
        // realme 7
        case 140:
        case 141:
        case 146:
        case 149:
            device_props = {"RMX2151", "RMX2151L1"};
            break;
        case 142:
            device_props = {"RMX2153", "RMX2153L1"};
            break;
        case 94:
        case 148:
            device_props = {"RMX2155", "RMX2155L1"};
            break;
        // realme Narzo 30 4G
        case 90:
        case 92:
            device_props = {"RMX2156", "RMX2156L1"};
            break;
        // realme Narzo 20 Pro
        case 143:
            device_props = {"RMX2161", "RMX2161L1"};
            break;
        case 145:
        case 147:
            device_props = {"RMX2163", "RMX2163L1"};
            break;
        default:
            LOG(ERROR) << "Unknown operator found: " << operator_code;
            device_props = {"", ""};
    }

    return device_props;
}

void set_device() {
    const DeviceProps device_props = get_device();

    if (!device_props.device.empty() && !device_props.model.empty()) {
        set_ro_build_prop("device", device_props.device);
        set_ro_build_prop("model", device_props.model);
        set_ro_build_prop("name", device_props.model);
        set_ro_build_prop("product", device_props.model, false);
    }
}

void load_dalvik_properties() {
    char const *heapstartsize;
    char const *heapgrowthlimit;
    char const *heapsize;
    char const *heapminfree;
    char const *heapmaxfree;
    char const *heaptargetutilization;
    struct sysinfo sys;

    sysinfo(&sys);

    if (sys.totalram >= 5ull * 1024 * 1024 * 1024) {
        // from - phone-xhdpi-6144-dalvik-heap.mk
        heapstartsize = "16m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.5";
        heapminfree = "8m";
        heapmaxfree = "32m";
    } else if (sys.totalram >= 3ull * 1024 * 1024 * 1024) {
        // from - phone-xhdpi-4096-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "192m";
        heapsize = "512m";
        heaptargetutilization = "0.6";
        heapminfree = "8m";
        heapmaxfree = "16m";
    } else {
        return;
    }

    property_override("dalvik.vm.heapstartsize", heapstartsize);
    property_override("dalvik.vm.heapgrowthlimit", heapgrowthlimit);
    property_override("dalvik.vm.heapsize", heapsize);
    property_override("dalvik.vm.heaptargetutilization", heaptargetutilization);
    property_override("dalvik.vm.heapminfree", heapminfree);
    property_override("dalvik.vm.heapmaxfree", heapmaxfree);
}

void vendor_load_properties() {
#ifndef __ANDROID_RECOVERY__
    set_device();
    load_dalvik_properties();
#endif
}
