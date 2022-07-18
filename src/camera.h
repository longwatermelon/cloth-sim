#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

struct Camera
{
    vec3 pos, rot;

    vec3 front, up, right;
};

struct Camera *cam_alloc(vec3 pos, vec3 rot);
void cam_free(struct Camera *c);

void cam_rot(struct Camera *c, vec3 rot);
void cam_update_vectors(struct Camera *c);

void cam_set_props(struct Camera *c, unsigned int shader);

void cam_view_mat(struct Camera *c, mat4 dest);

#endif

