project('libleet', 'cpp', version : '0.1.0', license : 'LGPL')

project_source_files = [
  'src/libleet.cpp',
  'src/net/Request.cpp',
  'src/crypto/olm.cpp',
]

include_directories = [
  include_directories('include'),
  include_directories('include/net'),
  include_directories('include/crypto'),
]

project_dependencies = [
  dependency('nlohmann_json', fallback : 'nlohmann_json'),
  dependency('openssl', fallback : 'openssl'),
  dependency('boost'),
]

project_build_args = [
  '-DLEET_VERSION=' + meson.project_version(),
]

if get_option('encryption')
  project_dependencies += [ dependency('olm', fallback : 'olm') ]
else
  project_build_args += [ '-DLEET_NO_ENCRYPTION=1' ]
endif

install_headers('include/libleet.hpp', subdir : 'libleet')
install_headers('include/net/Request.hpp', subdir : 'libleet/net')
install_headers('include/crypto/olm.hpp', subdir : 'libleet/crypto')

static_library('leet', project_source_files, dependencies : project_dependencies, install : true, include_directories : include_directories)
shared_library('leet', project_source_files, dependencies : project_dependencies, version : meson.project_version(), install : true, include_directories : include_directories)

import('pkgconfig').generate(libraries : '-lleet', subdirs : 'libleet', version : meson.project_version(), name : meson.project_name(), filebase : meson.project_name(), description : 'Matrix client library/SDK')
