project(
  'return-sync',
  'cpp',
  version : '0.1',
  default_options : ['warning_level=3']
)

project_source_files = [
  'return-sync.cpp',
]

project_dependencies = [
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
