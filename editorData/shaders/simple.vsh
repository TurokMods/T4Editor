#version 150 core
in vec3 position;
in vec3 normal;
in vec2 texc;

out vec3 norm;
out vec2 tex;

uniform mat4 model;
uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    norm = normal;
    tex = texc;
}
