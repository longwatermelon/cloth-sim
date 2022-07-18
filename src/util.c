#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *util_read_file(const char *path)
{
    char *contents = malloc(sizeof(char));
    contents[0] = '\0';

    FILE *fp = fopen(path, "r");
    char *line = 0;
    size_t len = 0;
    ssize_t read;

    if (!fp)
    {
        fprintf(stderr, "Couldn't read file %s\n", path);
        return 0;
    }

    while ((read = getline(&line, &len, fp)) != -1)
    {
        unsigned long prev_length = strlen(contents);

        char *nl = calloc(sizeof(char), ++read);
        strcat(nl, "\n");

        contents = realloc(contents, (strlen(contents) + read) * sizeof(char));

        memcpy(&contents[prev_length], line, (read - 1) * sizeof(char));
        contents[prev_length + read - 1] = '\0';
    }

    free(line);
    fclose(fp);

    return contents;
}


void util_quat_from_rot(vec3 rot, vec4 dest)
{
    vec4 yaw, pitch;
    util_eul2quat((vec3){ 0.f, 0.f, rot[2] }, yaw);
    util_eul2quat((vec3){ 0.f, rot[1], 0.f }, pitch);

    glm_quat_mul(yaw, pitch, dest);
}


void util_eul2quat(vec3 rot, vec4 dest)
{
    float yaw = rot[1];
    float pitch = rot[2];
    float roll = rot[0];

    float cy = cosf(yaw * .5f);
    float sy = sinf(yaw * .5f);
    float cp = cosf(pitch * .5f);
    float sp = sinf(pitch * .5f);
    float cr = cosf(roll * .5f);
    float sr = sinf(roll * .5f);

    dest[3] = cr * cp * cy + sr * sp * sy;
    dest[0] = sr * cp * cy - cr * sp * sy;
    dest[1] = cr * sp * cy + sr * cp * sy;
    dest[2] = cr * cp * sy - sr * sp * cy;
}

