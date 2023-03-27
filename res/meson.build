prog_python3 = find_program('python3')

gen = generator(prog_python3,
  output : '@PLAINNAME@.c',
  arguments : ['@SOURCE_DIR@/util/xxd.py', '@INPUT@'],
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
