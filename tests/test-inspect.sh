#!/usr/bin/env bash
# This test checks if the multidec plugin is correctly installed and can be inspected using gst-inspect-1.0. It does not perform any actual decoding, but ensures that the plugin is recognized by GStreamer.

set -euo pipefail

export GST_PLUGIN_PATH="$PWD/build/src"
# This command will exit with a non-zero status if the plugin is not found, causing the test to fail
gst-inspect-1.0 multidec | grep -q "Name[[:space:]]*multidec"