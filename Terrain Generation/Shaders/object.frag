#version 460 core

//in vec3 aCol;
in float height;

out vec4 FragColor;
uniform float minHeight, maxHeight;

float lerp(float a, float b, float x) {
	return a + x * (b-a);
}

float far = 300.0f;
float near = 0.1f;

float linearizeDepth(float depth)
{
	float ndc = depth * 2.0f - 1.0f;
	return (2.0f * near * far) / (far + near - ndc * (far - near));
}

void main()
{
	float frac = (height - minHeight) / (maxHeight - minHeight);
	float lerped = (lerp(minHeight, maxHeight, frac) - minHeight) / (maxHeight - minHeight); // btw 0 and 1

	vec4 col = vec4(lerped);
//	if (height == minHeight) {
//		col = vec4(0.0, 0.0, 1.0f, 1.0f);
//	} else if (height < 50.0f) {
//		col = vec4(0.0, 1.0, 0.0f, 1.0f);
//	} else if (height < 100.0f) {
//		col = vec4(0.537, 0.318, 0.161, 1.0f);
//	} else
//		col = vec4(1.0f);

	float depth = linearizeDepth(gl_FragCoord.z) / far;
	vec4 depthVec4 = vec4(vec3(pow(depth, 0.9)), 1.0);
	col = col * (1 - depthVec4) + depthVec4;

	col = pow(col, vec4(1.0f/2.2f));

	FragColor = col;
}