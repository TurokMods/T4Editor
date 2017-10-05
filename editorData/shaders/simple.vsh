#version 150 core
in vec3 position;
in vec3 normal;
in vec2 texc;

void main() {
    gl_Position = vec4(position * 0.2, 1.0);
}
