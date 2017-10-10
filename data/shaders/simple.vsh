#version 150 core
in vec3 position;
in vec3 normal;
in vec2 texc;

out vec3 norm;
out vec2 tex;
out vec3 lightDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 mvp;

void main() {
    mat3 NormalMatrix = transpose(inverse(mat3(view*model)));
    vec3 eyeNorm = normalize(NormalMatrix * normal);
    gl_Position =  mvp * vec4(position, 1.0);
    norm = eyeNorm;
    tex = texc;
    lightDir = mat3(view*model) * vec3(0.25, 1, 0);
}
