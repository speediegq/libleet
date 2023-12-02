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

pkgconfig = configuration_data()
pkgconfig.set('prefix', get_option('prefix'))
pkgconfig.set('libdir', join_paths(get_option('prefix'), 'lib'))
pkgconfig.set('includedir', join_paths(get_option('prefix'), 'include'))
pkgconfig.set('Name', meson.project_name())
pkgconfig.set('Version', meson.project_version())
pkgconfig.set('Libs', '-L${libdir} -lleet')
pkgconfig.set('Cflags', '-I${includedir}')
pkgconfig_file = configure_file(output : 'libleet.pc', configuration : pkgconfig, install : true, install_dir : join_paths(get_option('prefix'), 'lib', 'pkgconfig'))

install_data(pkgconfig_file)
