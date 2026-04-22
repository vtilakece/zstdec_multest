#!/usr/bin/env bash
# This test checks if the multidec plugin is correctly installed and can be inspected using gst-inspect-1.0. It does not perform any actual decoding, but ensures that the plugin is recognized by GStreamer.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
export GST_PLUGIN_PATH="$ROOT_DIR/build/src"



# This command will exit with a non-zero status if the plugin is not found, causing the test to fail
OUTPUT="$(gst-inspect-1.0 multidec)"
printf '%s\n' "$OUTPUT" | grep -q "Name[[:space:]]*multidec"