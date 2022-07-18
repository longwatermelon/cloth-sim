#ifndef SHADER_H
#define SHADER_H

#define shader_vec3(shader, name, value) glUniform3fv(glGetUniformLocation(shader, name), 1, value)
#define shader_mat4(shader, name, value) glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, value[0])
#define shader_float(shader, name, value) glUniform1f(glGetUniformLocation(shader, name), value)
#define shader_int(shader, name, value) glUniform1i(glGetUniformLocation(shader, name), value)

unsigned int shader_create(const char *vert, const char *frag);
unsigned int shader_compile(unsigned int type, const char *src);

#endif

