#ifndef __GFX_H__
#define __GFX_H__

#include <epoxy/gl.h>

#include "ui.h"

struct GLCtx {
	unsigned res_x;
	unsigned res_y;
};

void gfx_init(GtkBuilder *builder);

extern struct GLCtx g_gl_ctx;
extern GtkGLArea *g_glarea;

#endif /* __GFX_H__ */
