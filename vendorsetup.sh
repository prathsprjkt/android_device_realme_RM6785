#!/bin/bash

function apply_patch {
    if patch -d "$1" -p1 --dry-run <<<"$(curl -sL "$2")" >/dev/null; then
	    git -C "$1" apply --ignore-whitespace <<<"$(curl -sL "$2")"
    else
	    echo "Skipping the patch for '$1', as it is already applied."
    fi
}

# media: Import changes for codecs and OMX from ALPS T
apply_patch "frameworks/av" "https://github.com/crDroid-RM6785/android_frameworks_av/commit/b55638b54ec60254a71b0386aad8b41fd9497b22.patch"

# Fix the brightness slider curve on some devices
apply_patch "frameworks/base" "https://github.com/crDroid-RM6785/android_frameworks_base/commit/93e596ced304c38894c60a478e93a97a5c0a212e.patch"

# telephony: Conditionally force LTE carrier aggregation
apply_patch "frameworks/base" "https://github.com/crDroid-RM6785/android_frameworks_base/commit/287c968a7da2cd4c403f51a9611efcc4d059b89e.patch"

# Settings: Implement a toggle for LTE carrier aggregation
apply_patch "packages/apps/Settings" "https://github.com/crDroid-RM6785/android_packages_apps_Settings/commit/662e58bb2f0af58c3b2b2462f0a0f116456b38c5.patch"
