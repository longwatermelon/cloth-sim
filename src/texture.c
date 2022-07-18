#include "texture.h"
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>
#include <stb/stb_image.h>


struct Texture *tex_alloc(const char *path)
{
    struct Texture *t = malloc(sizeof(struct Texture));
    strcpy(t->path, path);

    glGenTextures(1, &t->id);

    int w, h, nchannels;
    unsigned char *data = stbi_load(path, &w, &h, &nchannels, 0);

    if (data)
    {
        GLenum format;

        switch (nchannels)
        {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            fprintf(stderr, "[tex_alloc] Expected 1, 3, or 4 channels in image '%s' but %d were given.\n", path, nchannels);
            exit(EXIT_FAILURE);
        }

        glBindTexture(GL_TEXTURE_2D, t->id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        fprintf(stderr, "[tex_alloc] Failed to load file '%s'.\n", path);
        exit(EXIT_FAILURE);
    }

    return t;
}


void tex_free(struct Texture *t)
{
    glDeleteTextures(1, &t->id);
    free(t);
}


void tex_bind(struct Texture *t, unsigned int slot)
{
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, t->id);
}

