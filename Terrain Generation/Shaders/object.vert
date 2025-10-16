#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 a_uv;

out float height;
out vec3 Normal;
out vec3 FragPos;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	height = texture(heightMap, a_uv).r * 256;
	gl_Position = projection * view * model * vec4(aPos.x, height, aPos.z ,1.0f);

	vec3 aNormal = vec3(1.0f);
	Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * gl_Position);
}