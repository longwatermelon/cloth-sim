#version 330 core
out vec4 FragColor;

in vec3 f_pos;
in vec3 f_norm;

void main()
{
    vec3 norm = normalize(f_norm);
    vec3 light = normalize(vec3(1., -.5, 0.));

    if (dot(light, norm) < 0)
        norm *= -1.;

    float diff = max(dot(norm, normalize(vec3(1., -.5, 0.))), 0.);

    FragColor = vec4(diff * vec3(.5, .5, 1.), 1.);
}

