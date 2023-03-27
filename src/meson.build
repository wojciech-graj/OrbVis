orbvis_sources = files([
  'core/system.c',
  'entity/earth.c',
  'entity/entity.c',
  'entity/satellite.c',
  'entity/sky.c',
  'gfx/bo.c',
  'gfx/camera.c',
  'gfx/error_gfx.c',
  'gfx/gfx.c',
  'gfx/idx_obj.c',
  'gfx/render.c',
  'gfx/shader.c',
  'gfx/texture.c',
  'gfx/vao.c',
  'main.c',
  'mainloop.c',
  'phys/phys.c',
  'ui/catalog.c',
  'ui/filter.c',
  'ui/info.c',
  'ui/input.c',
  'ui/perf.c',
  'ui/setting.c',
  'ui/status.c',
  'ui/toolbar.c',
  'ui/ui.c',
  'util/download.c',
  'util/icosphere.c',
  'util/model.c',
  'util/satcat_code_list.c',
  'util/thread.c',
])

orbvis_incdir = include_directories([
  '.',
  'core',
  'entity',
  'gfx',
  'phys',
  'ui',
  'util',
])

conf_data = configuration_data()
conf_data.set('version', meson.project_version())
configure_file(input: 'version.h.in',
  output : 'version.h',
  configuration : conf_data
)
