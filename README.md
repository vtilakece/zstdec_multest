# zstddec GStreamer Plugin

## Overview

`zstddec` is a simple GStreamer plugin that decompresses **Zstandard (zstd)**-compressed data streams.

It is implemented as a `GstBaseTransform` element and performs buffer-to-buffer decompression.

---

## Features

- Decompresses **Zstandard (.zst)** streams
- One **sink pad** (compressed input)
- One **source pad** (decompressed output)
- Uses **libzstd** for decompression
- Built using the **Meson** build system
- Compatible with Linux (x86-64)

---

## Dependencies

Install required packages:

```bash
sudo apt update
sudo apt install -y \
  meson ninja-build pkg-config build-essential \
  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
  libzstd-dev \
  gstreamer1.0-tools zstd
```
## Build instructions
```bash
meson setup build
meson compile -C build
```
## Inspecting the Plugin
```bash
export GST_PLUGIN_PATH=$PWD/build/src
gst-inspect-1.0 zstddec
```

## Decompress using GStreamer
```bash
gst-launch-1.0 \
  filesrc location=input.txt.zst ! \
  zstddec ! \
  filesink location=output.txt
```
## Testing
```bash
meson test -C build
```

## Project Structure

```
.
├── src/            # Plugin source code
├── include/        # Headers
├── tests/          # Test scripts and sample data
├── meson.build     # Build configuration
└── .github/        # CI workflow
```
## Notes
 - This branch implements only Zstandard decompression
 - Multi-format support is available in the multidec branch
 - Assumes complete frames per buffer (no streaming support)

## License

This project is licensed under the GNU Lesser General Public License v2.1 (LGPL-2.1).
You are free to use, modify, and distribute this software under the terms of the license.
 
  
