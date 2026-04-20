gst_dep = dependency('gstreamer-1.0', required : true)
# Base transform to pass through data as a filtr 
gstbase_dep = dependency('gstreamer-base-1.0', required : true)


# Optional for later use.
# If libzstd is installed with pkg-config support, Meson will find it.
zstd_dep = dependency('libzstd', required : true)

# This is to find the deps at compile time
deps = [gst_dep, gstbase_dep, zstd_dep ]
cargs = []

if zstd_dep.found()
  deps += zstd_dep
  cargs += ['-DHAVE_ZSTD=1']
endif

# Build a GStreamer plugin shared object.
incdir = include_directories('../include')

shared_module(
  'zstddec',
  'zstddec.c',
  include_directories : incdir,
  dependencies : deps,
  c_args : cargs,
  install : true,
  # Incase of manual gstreamer plugin, for later: -Dgstreamer_plugin_dir=/some/path
  install_dir : get_option('libdir') / 'gstreamer-1.0',
)

