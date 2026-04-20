# zstdec_multest
This is a plugin zstdec that decompresses the zst format files using gstreamer

Requirments:
Requirements (Mandatory)
The plugin must be named "zstddec".
The plugin must have one sink pad (input) and one source pad (output).
The plugin should decompress Zstandard-compressed streams.
The deliverable is the source code, organized so it can be compiled on a Linux system (x86-64 architecture).
The project must use the Meson build system.
At compilation time, the plugin must depend on the libzstd library via pkg-config.
The delivery must be a git repository, a tarball including the full git history, or a private repository on GitHub or a similar platform.
