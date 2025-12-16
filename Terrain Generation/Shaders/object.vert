#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 a_uv;

out float height;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform sampler2D heightMap;

uniform float yScale;
uniform float yShift;
uniform float seaLevel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
	height = texture(heightMap, a_uv).r * yScale - yShift;
	if (height < seaLevel) height = seaLevel;
	gl_Position = projection * view * model * vec4(aPos.x, height, aPos.z ,1.0f);

	float hL, hR, hU, hD;
	hL = hR = hD = hU = height;

	ivec2 map_size = textureSize(heightMap, 0);
	float uv_dx = 1.0f / map_size.x;
	float uv_dy = 1.0f / map_size.y;

	if (a_uv.x != 0)
		hL = texture(heightMap, vec2(a_uv.x - uv_dx, a_uv.y)).r * yScale - yShift;
	if (a_uv.x != map_size.x - 1)
		hR = texture(heightMap, vec2(a_uv.x + uv_dx, a_uv.y)).r * yScale - yShift;
	if (a_uv.y != 0)
		hD = texture(heightMap, vec2(a_uv.x, a_uv.y - uv_dy)).r * yScale - yShift;
	if (a_uv.y != map_size.y - 1)
		hU = texture(heightMap, vec2(a_uv.x, a_uv.y + uv_dy)).r * yScale - yShift;

	vec3 normal = vec3((hL - hR) / 2.0f, 1.0f, (hD - hU) / 2.0f);
	normal = normalize(normal);

	if (height <= seaLevel)
		normal = vec3(0.0f, 1.0f, 0.0f);

	Normal = mat3(transpose(inverse(model))) * normal;
	FragPos = vec3(model * vec4(aPos, 1.0f));
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
}