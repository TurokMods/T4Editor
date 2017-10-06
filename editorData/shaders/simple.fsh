#version 150 core
in vec3 norm;
in vec2 tex;

out vec4 outcolor;

void main() {
    outcolor = vec4(norm, 1);
}
