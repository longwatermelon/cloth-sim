#version 330 core
out vec4 FragColor;

in vec3 f_pos;
in vec3 f_color;

void main()
{
    FragColor = vec4(f_color, 1.);
}

