#version 460 core

in vec3 aCol;
in float height;



out vec4 FragColor;

void main()
{
	FragColor = vec4(aCol, 1.0f);
}