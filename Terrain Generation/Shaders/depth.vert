#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 a_uv;

uniform sampler2D heightMap;

uniform float yScale;
uniform float yShift;
uniform float seaLevel;

uniform mat4 lightSpaceMatrix;

void main()
{
	float height = texture(heightMap, a_uv).r * yScale - yShift;
	if (height < seaLevel) height = seaLevel;
	gl_Position = lightSpaceMatrix * vec4(aPos.x, height, aPos.z ,1.0f);
}