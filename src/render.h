#ifndef RENDER_H
#define RENDER_H

#include "camera.h"
#include <cglm/cglm.h>

enum
{
    SHADER_BASIC
};

typedef struct
{
    unsigned int *shaders;
    size_t nshaders;

    unsigned int shader;

    mat4 proj, view;
    struct Camera *cam;
} RenderInfo;

RenderInfo *ri_alloc();
void ri_free(RenderInfo *ri);

void ri_add_shader(RenderInfo *ri, const char *vert, const char *frag);

void ri_use_shader(RenderInfo *ri, int i);

#endif

