#ifndef MESH_H
#define MESH_H

#include "render.h"
#include <cglm/cglm.h>

typedef struct
{
    vec3 pos, col;
} Vertex;

struct Mesh
{
    Vertex *verts;
    size_t nverts;

    unsigned int *indices;
    size_t nindices;

    unsigned int vao, vb, ib;
};

struct Mesh *mesh_alloc();
void mesh_free(struct Mesh *m);

void mesh_construct(struct Mesh *m);

void mesh_render(struct Mesh *m, RenderInfo *ri);

#endif

