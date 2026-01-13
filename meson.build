project(
  'OrbVis',
  'c',
  version : '0.3.3',
  license : 'GPL-2.0-or-later',
  default_options : [
    'c_std=c11',
    'warning_level=3',
    'b_lto=true',
    'buildtype=release',
    'b_ndebug=if-release',
    'debug=false',
    'c_args=-DSC_CSTRING',
  ],
)

add_project_arguments(
  '-Wno-implicit-fallthrough',
  '-Wno-dangling-else',
  '-Wno-newline-eof',
  '-Wno-unknown-pragmas',
  language : ['c'],
)

cc = meson.get_compiler('c')

deps = [
  dependency('gtk+-3.0'),
  dependency('libcurl'),
  dependency('epoxy'),
  cc.find_library('m', required : false),
]

subdir('src')
subdir('lib')
subdir('res')

executable(
  'orbvis',
  orbvis_sources,
  lib_sources,
  gen_res,
  include_directories : [orbvis_incdir, lib_incdir],
  dependencies : deps,
  install : true,
)
