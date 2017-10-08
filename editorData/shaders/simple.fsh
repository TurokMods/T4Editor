#version 330 core
in vec3 norm;
in vec2 tex;
in vec3 lightDir;

uniform vec3 actor_id;
uniform vec3 actor_submesh_id;
uniform vec3 actor_submesh_chunk_id;
uniform float actor_selected;

layout(location = 0) out vec3 outcolor;
layout(location = 1) out vec3 out_actor_id;
layout(location = 2) out vec3 out_actor_submesh_id;
layout(location = 3) out vec3 out_actor_submesh_chunk_id;

void main() {
    float ndotl = max(dot(norm, lightDir), 0.0);
    outcolor = ndotl * norm;
    if(actor_selected > 0.0) {
        outcolor.r += 0.2;
        outcolor.g += 0.2;
        outcolor.b = actor_selected;
    }
    out_actor_id = actor_id;
    out_actor_submesh_id = actor_submesh_id;
    out_actor_submesh_chunk_id = actor_submesh_chunk_id;
}
