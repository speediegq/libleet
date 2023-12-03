project(
  'libleet',
  'cpp',
  version : '0.1',
  license : 'AGPL',
)

cc = meson.get_compiler('cpp')

project_source_files = [
  'src/libleet.cpp',
]

project_dependencies = [
  dependency('nlohmann_json'),
  dependency('cpr'),
  dependency('olm'),
  dependency('openssl'),
]

build_args = [
  '-DLLEET_VERSION=' + meson.project_version(),
]

so_version = meson.project_version()

install_headers('include/libleet.hpp', subdir : 'libleet')

lib = library('leet',
  project_source_files,
  dependencies : project_dependencies,
  version : so_version,
  install : true
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(libraries : '-lleet', subdirs : 'libleet', version : so_version, name : meson.project_name(), filebase : meson.project_name(), description : 'Matrix client library')
