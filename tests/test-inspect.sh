#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
export GST_PLUGIN_PATH="$ROOT_DIR/build/src"
# Check if the zstddec plugin is available and exits if non zero status is returned
OUTPUT="$(gst-inspect-1.0 zstddec)"
printf '%s\n' "$OUTPUT" | grep -q "Name[[:space:]]*zstddec"