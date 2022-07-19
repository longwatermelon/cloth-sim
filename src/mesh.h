#ifndef MESH_H
#define MESH_H

#include "render.h"
#include <cglm/cglm.h>

typedef struct
{
    vec3 pos, norm;
} Vertex;

struct Mass
{
    float mass;
    vec3 vel;

    Vertex *vert;
};

void mass_apply_force(struct Mass *m, vec3 f, float dt);

struct Spring
{
    struct Mass *ma, *mb;
    float k, eq_len;
};

void spring_force(struct Spring *s, struct Mass *m, vec3 out);

struct Mesh
{
    int size;
    float res;

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

struct Mesh *mesh_alloc(int size, float res);
void mesh_free(struct Mesh *m);

void mesh_update(struct Mesh *m, float dt, size_t *held, size_t nheld);
void mesh_render(struct Mesh *m, RenderInfo *ri);

void mesh_calculate_normals(struct Mesh *m);

void mesh_construct(struct Mesh *m);
void mesh_gen_springs(struct Mesh *m);

#endif

