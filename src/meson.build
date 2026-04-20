gst_dep = dependency('gstreamer-1.0', required : true)
gstbase_dep = dependency('gstreamer-base-1.0', required : true)
zstd_dep = dependency('libzstd', required : true)

# optional extra-credit deps
zlib_dep = dependency('zlib', required : false)

cc = meson.get_compiler('c')

# Try pkg-config/CMake first; fall back to linking libbz2 directly.
bzip2_dep = dependency('bzip2', required : false)
if not bzip2_dep.found()
  bzip2_dep = cc.find_library('bz2', required : false)
endif

deps = [gst_dep, gstbase_dep, zstd_dep]
cargs = ['-DHAVE_ZSTD=1']

if zlib_dep.found()
  deps += zlib_dep
  cargs += ['-DHAVE_ZLIB=1']
endif

if bzip2_dep.found()
  deps += bzip2_dep
  cargs += ['-DHAVE_BZIP2=1']
endif

incdir = include_directories('../include')

shared_module(
  'multidec',
  'multidec.c',
  include_directories : incdir,
  dependencies : deps,
  c_args : cargs,
  install : true,
  install_dir : get_option('libdir') / 'gstreamer-1.0',
)