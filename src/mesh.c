#include "mesh.h"
#include "shader.h"
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

void mass_apply_force(struct Mass *m, vec3 f, float dt)
{
    // dp = Ft
    vec3 dp;
    glm_vec3_scale(f, dt, dp);

    // v += dp / m
    vec3 dv;
    glm_vec3_divs(dp, m->mass, dv);
    glm_vec3_add(m->vel, dv, m->vel);
}

void spring_force(struct Spring *s, struct Mass *m, vec3 out)
{
    struct Mass *m2 = (m == s->ma ? s->mb : s->ma);

    float dist = glm_vec3_distance(m->vert->pos, m2->vert->pos);
    float left = s->k * (s->eq_len - dist);

    vec3 diff;
    glm_vec3_sub(m->vert->pos, m2->vert->pos, diff);
    glm_vec3_divs(diff, dist, diff);

    glm_vec3_scale(diff, left, out);
}

struct Mesh *mesh_alloc(int size, float res)
{
    struct Mesh *m = malloc(sizeof(struct Mesh));
    m->size = size;
    m->res = res;

    m->verts = 0;
    m->nverts = 0;
    m->indices = 0;
    m->nindices = 0;
    m->masses = 0;
    m->nmasses = 0;
    m->springs = 0;
    m->nsprings = 0;

    mesh_construct(m);
    mesh_gen_springs(m);

    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    glGenBuffers(1, &m->vb);
    glBindBuffer(GL_ARRAY_BUFFER, m->vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m->nverts, m->verts, GL_STATIC_DRAW);

    glGenBuffers(1, &m->ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m->nindices, m->indices, GL_STATIC_DRAW);

    // verts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return m;
}


void mesh_free(struct Mesh *m)
{
    free(m->verts);
    free(m->indices);
    free(m->masses);
    free(m->springs);

    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vb);

    free(m);
}


void mesh_update(struct Mesh *m, float dt, size_t *held, size_t nheld)
{
    for (size_t i = 0; i < m->nsprings; ++i)
    {
        struct Spring *s = &m->springs[i];

        vec3 fa, fb;
        spring_force(s, s->ma, fa);
        spring_force(s, s->mb, fb);

        mass_apply_force(s->ma, fa, dt);
        mass_apply_force(s->mb, fb, dt);
    }

    for (size_t i = 0; i < m->nmasses; ++i)
    {
        /* if (i == m->size * m->size - 1 || i == m->size * m->size - m->size) */
        bool cont = false;
        for (size_t j = 0; j < nheld; ++j)
        {
            if (i == held[j])
                cont = true;
        }

        if (cont) continue;
        /* if (i == 35 || i == 1022) */
        /*     continue; */

        {
            // gravity
            vec3 fg = { 0.f, -9.8f * m->masses[i].mass, 0.f };
            mass_apply_force(&m->masses[i], fg, dt);
        }

        {
            // air resistance
            vec3 drag;

            vec3 vsq;

            // Preserve sign in vsq
            vec3 sign;
            glm_vec3_sign(m->masses[i].vel, sign);

            glm_vec3_mul(m->masses[i].vel, m->masses[i].vel, vsq);
            glm_vec3_mul(vsq, sign, vsq);

            glm_vec3_copy(m->masses[i].vel, vsq);

            glm_vec3_scale(vsq, .01f, drag);
            glm_vec3_sub(m->masses[i].vel, drag, m->masses[i].vel);
        }

        vec3 move;
        glm_vec3_scale(m->masses[i].vel, dt, move);
        glm_vec3_add(m->masses[i].vert->pos, move, m->masses[i].vert->pos);
    }

    mesh_calculate_normals(m);

    glBindBuffer(GL_ARRAY_BUFFER, m->vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m->nverts * sizeof(Vertex), m->verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void mesh_render(struct Mesh *m, RenderInfo *ri)
{
    mat4 model;
    glm_mat4_identity(model);

    shader_mat4(ri->shader, "model", model);

    glBindVertexArray(m->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
    glDrawElements(GL_TRIANGLES, m->nindices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static bool in_range(struct Mesh *m, size_t a, size_t b)
{
    return a >= 0 && a < m->nverts &&
           b >= 0 && b < m->nverts;
}

static bool compute_face_norm(struct Mesh *m, size_t a, size_t b, vec3 out)
{
    if (in_range(m, a, b))
    {
        glm_vec3_cross(m->verts[a].pos, m->verts[b].pos, out);
        return true;
    }

    glm_vec3_zero(out);
    return false;
}

void mesh_calculate_normals(struct Mesh *m)
{
    for (size_t i = 0; i < m->nindices; ++i)
    {
        size_t index = m->indices[i];
        int s = m->size;

        vec3 norms[6];
        bool included[6];

        included[0] = compute_face_norm(m, index - s - 1,  index - 1,      norms[0]);
        included[1] = compute_face_norm(m, index - s,      index - s - 1,  norms[1]);
        included[2] = compute_face_norm(m, index + 1,      index - s,      norms[2]);
        included[3] = compute_face_norm(m, index + s + 1,  index + 1,      norms[3]);
        included[4] = compute_face_norm(m, index + s,      index + s + 1,  norms[4]);
        included[5] = compute_face_norm(m, index - 1,      index + s,      norms[5]);

        vec3 avg = { 0.f, 0.f, 0.f };

        int count = 0;

        for (int j = 0; j < 6; ++j)
        {
            if (included[j])
            {
                glm_vec3_add(avg, norms[j], avg);
                ++count;
            }
        }

        glm_vec3_divs(avg, (float)count, m->verts[index].norm);
    }
}


void mesh_construct(struct Mesh *m)
{
    m->verts = malloc(sizeof(Vertex) * (m->size * m->size));

    for (int y = 0; y < m->size; ++y)
    {
        for (int z = 0; z < m->size; ++z)
        {
            Vertex v = {
                { (float)y * m->res, -.4f, (float)z * m->res },
                { 0.f, 1.f, 0.f }
            };

            m->verts[m->nverts++] = v;

            m->masses = realloc(m->masses, sizeof(struct Mass) * ++m->nmasses);
            m->masses[m->nmasses - 1] = (struct Mass){ .5f, { 0.f, 0.f, 0.f }, &m->verts[m->nverts - 1] };

            if (y != m->size - 1 && z != m->size - 1)
            {
                unsigned int i = y * m->size + z;
                unsigned int ta[3], tb[3];

                ta[0] = i;
                ta[1] = i + m->size + 1;
                ta[2] = i + m->size;

                tb[0] = i;
                tb[1] = i + 1;
                tb[2] = i + m->size + 1;

                size_t ia = m->nindices;
                size_t ib = m->nindices + 3;

                m->nindices += 6;
                m->indices = realloc(m->indices, sizeof(unsigned int) * m->nindices);

                memcpy(m->indices + ia, ta, sizeof(unsigned int) * 3);
                memcpy(m->indices + ib, tb, sizeof(unsigned int) * 3);
            }
        }
    }
}


void mesh_gen_springs(struct Mesh *m)
{
    // edges + crosses
    int area = (m->size - 1) * (m->size - 1); // m->size counts vertices not squares
    m->nsprings = (area * 2 + (m->size - 1) * 2) + (area * 2);
    m->springs = malloc(sizeof(struct Spring) * m->nsprings);

    size_t index = 0;

    float k = 1200.f;
    float eq_len = m->res;
    float eq_len_diag = sqrtf(m->res * m->res * 2.f);

    // horizontal and vertical
    for (size_t y = 0; y < m->size; ++y)
    {
        for (size_t z = 0; z < m->size - 1; ++z)
        {
            size_t mi = y * m->size + z;
            m->springs[index++] = (struct Spring){ &m->masses[mi], &m->masses[mi + 1], k, eq_len };
        }
    }

    for (size_t y = 0; y < m->size - 1; ++y)
    {
        for (size_t z = 0; z < m->size; ++z)
        {
            size_t mi = y * m->size + z;
            m->springs[index++] = (struct Spring){ &m->masses[mi], &m->masses[mi + m->size], k, eq_len };
        }
    }

    // diagonal
    for (size_t y = 0; y < m->size - 1; ++y)
    {
        for (size_t z = 0; z < m->size - 1; ++z)
        {
            size_t mi = y * m->size + z;
            m->springs[index++] = (struct Spring){ &m->masses[mi], &m->masses[mi + m->size + 1], k, eq_len_diag };
            m->springs[index++] = (struct Spring){ &m->masses[mi + m->size], &m->masses[mi + 1], k, eq_len_diag };
        }
    }
}

