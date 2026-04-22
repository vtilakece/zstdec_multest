# zstddec GStreamer Plugin

## Overview

`multidec` is a GStreamer plugin that decompresses compressed data streams in multiple formats.

It supports:

- **Zstandard (zstd)**
- **gzip (zlib)**
- **bzip2**

The plugin can either **auto-detect** the compression format from the input stream or use a user-specified format.

It is implemented as a `GstBaseTransform` element and performs buffer-to-buffer decompression.

---

## Features

- Supports multiple compression formats:
  - Zstandard (.zst)
  - gzip (.gz)
  - bzip2 (.bz2)
- **Automatic format detection** based on input header
- Manual format selection via property
- One **sink pad** (compressed input)
- One **source pad** (decompressed output)
- Built using the **Meson** build system
- Compatible with Linux (x86-64)
 
---

## Supported Formats

| Format   | Detection | Library Used |
|----------|----------|--------------|
| zstd     | Yes      | libzstd      |
| gzip     | Yes      | zlib         |
| bzip2    | Yes      | libbz2       |

---

## Dependencies

Install required packages:

```bash
sudo apt update
sudo apt install -y \
  meson ninja-build pkg-config build-essential \
  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
  libzstd-dev zlib1g-dev libbz2-dev \
  gstreamer1.0-tools gzip bzip2 zstd
```
## Build instructions
```bash
meson setup build
meson compile -C build
```
## Inspecting the Plugin
```bash
export GST_PLUGIN_PATH=$PWD/build/src
gst-inspect-1.0 multidec
```

## Usage Examples

### Auto-detect format

```bash
gst-launch-1.0 \
  filesrc location=input.txt.zst ! \
  multidec format=auto ! \
  filesink location=output.txt
```

---

### Force zstd

```bash
gst-launch-1.0 \
  filesrc location=input.txt.zst ! \
  multidec format=zstd ! \
  filesink location=output.txt
```

---

### Force gzip

```bash
gst-launch-1.0 \
  filesrc location=input.txt.gz ! \
  multidec format=gzip ! \
  filesink location=output.txt
```

---

### Force bzip2

```bash
gst-launch-1.0 \
  filesrc location=input.txt.bz2 ! \
  multidec format=bzip2 ! \
  filesink location=output.txt
```

---


## Format Property

The plugin exposes a `format` property:

| Value   | Description |
|--------|------------|
| auto   | Detect format from input (default) |
| zstd   | Force Zstandard decoding |
| gzip   | Force gzip decoding |
| bzip2  | Force bzip2 decoding |

---

## Testing
```bash
meson test -C build
```
Tests verify:

- Plugin is discoverable via `gst-inspect`
- Decompression output matches original input
- All supported formats work correctly
- Auto-detection works as expected
  
---

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
- The plugin currently assumes **complete frames per buffer**
  - Each input buffer must contain a full compressed frame
  - Streaming (partial-buffer) decoding is not supported
- Designed primarily for file-based pipelines
- Error handling is implemented for invalid or suspicious frame sizes

## License

This project is licensed under the GNU Lesser General Public License v2.1 (LGPL-2.1).
You are free to use, modify, and distribute this software under the terms of the license.
This plugin is developed for the GStreamer multimedia framework and follows its licensing model.
 
  
