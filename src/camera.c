#include "camera.h"
#include "shader.h"
#include "util.h"
#include <glad/glad.h>


struct Camera *cam_alloc(vec3 pos, vec3 rot)
{
    struct Camera *c = malloc(sizeof(struct Camera));
    glm_vec3_dup(pos, c->pos);
    cam_rot(c, rot);

    return c;
}


void cam_free(struct Camera *c)
{
    free(c);
}


void cam_rot(struct Camera *c, vec3 rot)
{
    glm_vec3_add(c->rot, rot, c->rot);
    cam_update_vectors(c);
}


void cam_update_vectors(struct Camera *c)
{
    vec4 quat;
    util_quat_from_rot(c->rot, quat);

    vec3 front = { 1.f, 0.f, 0.f };
    glm_quat_rotatev(quat, front, front);

    vec3 world_up;
    glm_quat_rotatev(quat, (vec3){ 0.f, 1.f, 0.f }, world_up);

    vec3 right, up;
    glm_cross(front, world_up, right);
    glm_cross(right, front, up);

    glm_vec3_normalize_to(front, c->front);
    glm_vec3_normalize_to(up, c->up);
    glm_vec3_normalize_to(right, c->right);
}


void cam_set_props(struct Camera *c, unsigned int shader)
{
    shader_vec3(shader, "viewPos", c->pos);
}


void cam_view_mat(struct Camera *c, mat4 dest)
{
    glm_look(c->pos, c->front, c->up, dest);
}

