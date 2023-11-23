#!/bin/bash

function apply_patch {
    if patch -d "$1" -p1 --dry-run <<<"$(curl -sL "$2")" >/dev/null; then
	    git -C "$1" apply --ignore-whitespace <<<"$(curl -sL "$2")"
    else
	    echo "Skipping the patch for '$1', as it is already applied."
    fi
}
