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
  dependency('openssl'),
]

build_args = [
  '-DLLEET_VERSION=' + meson.project_version(),
]

if get_option('encryption')
  project_dependencies += [ dependency('olm') ]
  project_dependencies += [ dependency('openssl') ]
  build_args += [ '-DENCRYPTION' ]
endif

so_version = meson.project_version()

install_headers('src/libleet.hpp', subdir : 'libleet')
lib = shared_library('leet', project_source_files, dependencies : project_dependencies, version : so_version, install : true)
