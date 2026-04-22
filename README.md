# zstdec_multest
This repo satisfies the follwoing requirements on two different branches 

### Requirement -> 1 `"zstddec"` plugin

- The plugin must be named `"zstddec"`.
- The plugin must have **one sink pad** (input) and **one source pad** (output).
- The plugin should **decompress Zstandard-compressed streams**.
- The deliverable is the source code, organized so it can be compiled on a Linux system (x86-64 architecture).
- The project must use the **Meson** build system.
- At compilation time, the plugin must depend on the **libzstd** library via `pkg-config`.


### Requirement -> 2 `"multidec"` plugin

- Support decompressing **bzip2** and/or **gzip** streams, in addition to zstd.
    - By supporting these formats, the plugin may be named generically (e.g., `multidec`), and should **autodetect the compression format**.
    - For gzip: depend on the zlib library (`pkg-config`).
    - For bzip2: depend on the bzip2 library (`pkg-config`).

## Notes
 - Requirmenet 1 is implemented in the branch `"zstdec"`. and  Requirement 2 is in `"multidec"` branch 
 - Continuous integration (CI) tests for the plugins are available 

