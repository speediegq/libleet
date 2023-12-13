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
  dependency('nlohmann_json', fallback : 'nlohmann_json'),
  dependency('openssl', fallback : 'openssl'),
  dependency('olm', fallback : 'olm'),
  dependency('boost'),
]

build_args = [
  '-DLLEET_VERSION=' + meson.project_version(),
]

so_version = meson.project_version()

install_headers('include/libleet.hpp', subdir : 'libleet')
install_headers('include/net/Request.hpp', subdir : 'libleet')

lib = static_library('leet',
  project_source_files,
  dependencies : project_dependencies,
  install : true
)

lib_shared = shared_library('leet',
  project_source_files,
  dependencies : project_dependencies,
  version : so_version,
  install : true
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(libraries : '-lleet', subdirs : 'libleet', version : so_version, name : meson.project_name(), filebase : meson.project_name(), description : 'Matrix client library')
