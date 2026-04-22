#!/usr/bin/env bash
set -euo pipefail

export GST_PLUGIN_PATH="$PWD/build/src"
# Exits with an error if the zstddec plugin is not found on a non zero exit code
gst-inspect-1.0 zstddec | grep -q "Name[[:space:]]*zstddec"