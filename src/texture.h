#ifndef TEXTURE_H
#define TEXTURE_H

#include "shader.h"
#include <limits.h>

struct Texture
{
    unsigned int id;

    char path[PATH_MAX];
};

struct Texture *tex_alloc(const char *path);
void tex_free(struct Texture *t);

void tex_bind(struct Texture *t, unsigned int slot);

#endif

