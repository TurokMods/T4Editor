#version 150 core
in vec3 norm;
in vec2 tex;
in vec3 lightDir;

out vec4 outcolor;

void main() {
    float ndotl = max(dot(norm, lightDir), 0.0);
    outcolor = ndotl * vec4(1.0, 0.0, 0.0, 1.0);
}
