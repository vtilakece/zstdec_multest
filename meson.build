project(
  'zstdec_multest',
  'c',
  version : '0.1.0',
  default_options : [
    'warning_level=2',
    'c_std=c11',
  ],
)

subdir('src')

test(
  'inspect-multidec',
  find_program('tests/test-inspect.sh')
)
test(
  'decompress-multidec',
  find_program('tests/test-decompress.sh')
)