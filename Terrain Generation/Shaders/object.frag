#version 460 core

in vec3 Normal;
in float height;
in vec3 FragPos;

uniform float minHeight, maxHeight;
uniform vec3 viewPos;

float far = maxHeight - minHeight;
float near = 0.1f;

out vec4 FragColor;

float linearizeDepth(float depth)
{
	float ndc = depth * 2.0f - 1.0f;
	return (2.0f * near * far) / (far + near - ndc * (far - near));
}

float lerp(float a, float b, float x) {
	return a + x * (b-a);
}

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dir;

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseColor);

void main()
{
	float frac = (height - minHeight) / (maxHeight - minHeight);
	vec4 col = vec4(vec3(frac),1.0f);
//	vec4 col = calculateDirLightCol(dir, vec4(vec3(frac), 1.0f));

	float param = abs(dot(normalize(Normal), vec3(0.0,1.0,0.0))); //btw 0,1

	//determine by altitude
	if (frac > 0.4)
		col = vec4(1.0f); //snow
	else if (frac == 0)
		col = vec4(0.0,0.53,0.74,1.0f);
	else 
		col = vec4(0.13,0.55,0.13,1.0f);

	//determine by slope
	if (frac != 0) {
		if (param <= 0.60)
			col = vec4(0.3,0.3,0.3,1.0f);
		else if (param <= 0.60)
			col = vec4(0.59,0.29,0.0,1.0f);
	}

	  // Blinn-Phong lighting
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lighting = calculateDirLight(dir, norm, viewDir, col.rgb);

    vec4 result = vec4(lighting, 1.0);
	
	//fog
//	float depth = linearizeDepth(gl_FragCoord.z) / far;
//	vec4 depthVec4 = vec4(vec3(pow(depth, 1.2)), 1.0);
//	result = result * (1 - depthVec4) + depthVec4;

	//gamma correction
	result = pow(result, vec4(1.0f/2.2f));

	FragColor = result;
}

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(-light.direction); // directional light
    
    // Ambient
    vec3 ambient = light.ambient * baseColor;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * baseColor;
//    
//    // Specular (Blinn-Phong uses halfway vector)
//    vec3 halfwayDir = normalize(lightDir + viewDir);
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0); 
//    vec3 specular = light.specular * spec;
    
    return ambient + diffuse;
}