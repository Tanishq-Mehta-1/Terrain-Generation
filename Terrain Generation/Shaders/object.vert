#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out float height;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos,1.0f);


	//float col = (aPos.y + 256.0f) / 512.0f;
	//aCol = vec3(col);
	//height = length(view * model * vec4(aPos.y));
	height = aPos.y;
	normal = aNormal;
}