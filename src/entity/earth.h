#ifndef __EARTH_H__
#define __EARTH_H__

#include "idx_obj.h"
#include "vao.h"

void earth_init(void);
void earth_deinit(void);
void earth_phys(void);
void earth_render(void);

extern struct IdxObj g_obj_earth;
extern struct VAO g_vao_earth;

#endif
