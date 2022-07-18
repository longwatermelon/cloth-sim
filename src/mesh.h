#ifndef MESH_H
#define MESH_H

#include "render.h"
#include <cglm/cglm.h>

typedef struct
{
    vec3 pos, col;
} Vertex;

struct Mass
{
    float mass;
    vec3 vel;

    Vertex *vert;
};

struct Spring
{
    struct Mass *ma, *mb;
    float k, eq_len;
};

struct Mesh
{
    int size;

    Vertex *verts;
    size_t nverts;

    struct Mass *masses;
    size_t nmasses;

    struct Spring *springs;
    size_t nsprings;

    unsigned int *indices;
    size_t nindices;

    unsigned int vao, vb, ib;
};

struct Mesh *mesh_alloc();
void mesh_free(struct Mesh *m);

void mesh_render(struct Mesh *m, RenderInfo *ri);

void mesh_construct(struct Mesh *m);
void mesh_gen_springs(struct Mesh *m);

#endif

