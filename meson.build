project(
  'libleet',
  'cpp',
  version : '"1.0.0"',
  license : 'GPL',
)

v_c=1
v_r=0
v_a=0

cc = meson.get_compiler('cpp')

project_source_files = [
  'src/libleet.cpp',
]

project_dependencies = [
  dependency('nlohmann_json'),
  dependency('cpr'),
]

build_args = [
  '-DLLEET_VERSION=' + meson.project_version(),
]

ll_lt_c=1
ll_lt_r=0
ll_lt_a=0

ll_so_version = '@0@.@1@.@2@'.format((ll_lt_c - ll_lt_a),
                                              ll_lt_a,
                                              ll_lt_r)

lib = shared_library('leet', project_source_files, version : ll_so_version, install : true)
