prog_xxd = find_program('xxd')
prog_sed = find_program('sed')
prog_sh = find_program('sh')

gen = generator(prog_sh,
  output : '@PLAINNAME@.c',
  arguments : ['@CURRENT_SOURCE_DIR@/gen_c.sh', '@INPUT@'],
  capture : true,
)

gen_res = gen.process([
  'shader/earth.frag',
  'shader/earth.vert',
  'shader/satellite.frag',
  'shader/satellite.vert',
  'shader/sky.frag',
  'shader/sky.vert',
  'texture/earth_clouds.jpg',
  'texture/earth_daymap.jpg',
  'texture/earth_nightmap.jpg',
  'texture/earth_specular_map.jpg',
  'texture/sky_gradient.jpg',
  'texture/stars.jpg',
  'ui/ui.glade',
])
