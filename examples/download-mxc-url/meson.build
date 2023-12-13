project(
  'download-mxc-url',
  'cpp',
  version : '"0.1"',
  default_options : ['warning_level=3']
)

cc = meson.get_compiler('cpp')

project_source_files = [
  'download-mxc-url.cpp',
]

project_dependencies = [
  dependency('nlohmann_json', fallback : 'nlohmann_json'),
  dependency('boost'),
  dependency('openssl', fallback : 'openssl'),
  dependency('olm', fallback : 'olm'),
]

build_args = [
  '-DVERSION=' + meson.project_version(),
]

project_target = executable(
  meson.project_name(),
  project_source_files, install : true,
  dependencies: project_dependencies,
  c_args : build_args,
)

test('download-mxc-url', project_target)
