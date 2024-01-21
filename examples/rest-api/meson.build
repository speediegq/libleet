project(
  'rest-api',
  'cpp',
  version : '0.1',
  default_options : ['warning_level=3']
)

project_source_files = [
  'rest-api.cpp',
]

project_dependencies = [
  dependency('nlohmann_json', fallback : 'nlohmann_json'),
  dependency('boost'),
  dependency('libleet'),
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

test(meson.project_name(), project_target)
