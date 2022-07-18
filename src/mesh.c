#include "mesh.h"
#include "shader.h"
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>


struct Mesh *mesh_alloc()
{
    struct Mesh *m = malloc(sizeof(struct Mesh));

    m->verts = 0;
    m->nverts = 0;
    m->indices = 0;
    m->nindices = 0;

    mesh_construct(m);

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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
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

    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vb);

    free(m);
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


void mesh_construct(struct Mesh *m)
{
    for (int y = 0; y < 10; ++y)
    {
        for (int z = 0; z < 10; ++z)
        {
            Vertex v = {
                { 5.f, y, z },
                { (float)(y * 10) / 100.f, 0.f, 1.f - (float)(z * 10) / 100.f }
            };

            m->verts = realloc(m->verts, sizeof(Vertex) * ++m->nverts);
            m->verts[m->nverts - 1] = v;

            if (y != 9 && z != 9)
            {
                unsigned int i = y * 10 + z;
                unsigned int ta[3], tb[3];

                ta[0] = i;
                ta[1] = i + 11;
                ta[2] = i + 10;

                tb[0] = i;
                tb[1] = i + 1;
                tb[2] = i + 11;

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

