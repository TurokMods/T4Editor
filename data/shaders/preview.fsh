#version 330 core
in vec3 norm;
in vec2 tex;
in vec3 lightDir;

uniform sampler2D diffuse_map;

layout(location = 0) out vec4 outcolor;

void main() {
    float ndotl = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = texture(diffuse_map, tex);
	if(diffuse.a < 0.5) discard;
	if(ndotl < 0.5) ndotl = 0.5;

	outcolor = vec4(ndotl * vec3(diffuse), 1.0);
}
