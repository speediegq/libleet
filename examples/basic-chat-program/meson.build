project(
  'basic-chat-program',
  'cpp',
  version : '"0.1"',
  default_options : ['warning_level=3']
)

cc = meson.get_compiler('cpp')

project_source_files = [
  'basic-chat-program.cpp',
]

project_dependencies = [
  dependency('nlohmann_json', fallback : 'nlohmann_json'),
  dependency('cpr', fallback : 'cpr'),
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

test('basic-chat-program', project_target)
