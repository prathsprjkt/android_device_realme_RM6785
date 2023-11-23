#!/bin/bash

function apply_patch {
    if patch -d "$1" -p1 --dry-run <<<"$(curl -sL "$2")" >/dev/null; then
	    git -C "$1" apply --ignore-whitespace <<<"$(curl -sL "$2")"
    else
	    echo "Skipping the patch for '$1', as it is already applied."
    fi
}

# media: Import changes for codecs and OMX from ALPS T
apply_patch "frameworks/av" "https://github.com/crDroid-RM6785/android_frameworks_av/commit/97c4052b1f1058a6bc9cc6752dcc6ba2be911cc0.patch"

# Fix the brightness slider curve on some devices
apply_patch "frameworks/base" "https://github.com/crDroid-RM6785/android_frameworks_base/commit/2bf736e7f29ab1daee7f02f264e6d8b139be7e24.patch"

# telephony: Conditionally force LTE carrier aggregation
apply_patch "frameworks/base" "https://github.com/crDroid-RM6785/android_frameworks_base/commit/67f38cf7f9c28ffb91d154109b851a17fe525b89.patch"

# Settings: Implement a toggle for LTE carrier aggregation
apply_patch "packages/apps/Settings" "https://github.com/crDroid-RM6785/android_packages_apps_Settings/commit/c0cb7314800cfcd25b811597307d62aee08b5d55.patch"
