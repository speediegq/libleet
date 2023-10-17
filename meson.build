project(
  'libleet',
  'cpp',
  version : '0.1',
  license : 'GPL',
)

cc = meson.get_compiler('cpp')

project_source_files = [
  'src/libleet.cpp',
]

project_dependencies = [
  dependency('nlohmann_json'),
  dependency('cpr'),
  dependency('olm'),
]

build_args = [
  '-DLLEET_VERSION=' + meson.project_version(),
]

so_version = meson.project_version()

install_headers('src/libleet.hpp', subdir : 'libleet')
lib = shared_library('leet', project_source_files, dependencies : project_dependencies, version : so_version, install : true)
