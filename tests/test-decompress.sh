#!/usr/bin/env bash
# Just exits when one of the commands fails, when an undefined variable is used, or when a command in a pipeline fails
set -euo pipefail
# Placeholders for the test data and output directories
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DATA_DIR="$ROOT_DIR/tests/data"
OUT_DIR="$ROOT_DIR/build/test-output"

mkdir -p "$OUT_DIR"
export GST_PLUGIN_PATH="$ROOT_DIR/build/src"

# zstd forced
gst-launch-1.0 -q \
  filesrc location="$DATA_DIR/README.txt.zst" ! \
  multidec format=zstd ! \
  filesink location="$OUT_DIR/out_zstd.txt"

# Compare the output with the original file
diff -u "$DATA_DIR/README.txt" "$OUT_DIR/out_zstd.txt"

# gzip forced
gst-launch-1.0 -q \
  filesrc location="$DATA_DIR/README.txt.gz" ! \
  multidec format=gzip ! \
  filesink location="$OUT_DIR/out_gzip.txt"

diff -u "$DATA_DIR/README.txt" "$OUT_DIR/out_gzip.txt"

# bzip2 forced
gst-launch-1.0 -q \
  filesrc location="$DATA_DIR/README.txt.bz2" ! \
  multidec format=bzip2 ! \
  filesink location="$OUT_DIR/out_bzip2.txt"

diff -u "$DATA_DIR/README.txt" "$OUT_DIR/out_bzip2.txt"

# auto detect
gst-launch-1.0 -q \
  filesrc location="$DATA_DIR/README.txt.zst" ! \
  multidec format=auto ! \
  filesink location="$OUT_DIR/out_auto_zstd.txt"

diff -u "$DATA_DIR/README.txt" "$OUT_DIR/out_auto_zstd.txt"

gst-launch-1.0 -q \
  filesrc location="$DATA_DIR/README.txt.gz" ! \
  multidec format=auto ! \
  filesink location="$OUT_DIR/out_auto_gzip.txt"

diff -u "$DATA_DIR/README.txt" "$OUT_DIR/out_auto_gzip.txt"

gst-launch-1.0 -q \
  filesrc location="$DATA_DIR/README.txt.bz2" ! \
  multidec format=auto ! \
  filesink location="$OUT_DIR/out_auto_bzip2.txt"

diff -u "$DATA_DIR/README.txt" "$OUT_DIR/out_auto_bzip2.txt"

echo "All decompression tests passed."