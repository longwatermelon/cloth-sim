#version 330 core
layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_norm;

out vec3 f_pos;
out vec3 f_norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    f_pos = vec3(model * vec4(i_pos, 1.));
    f_norm = i_norm;
    gl_Position = projection * view * vec4(i_pos, 1.);
}

