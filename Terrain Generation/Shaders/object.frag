#version 460 core

in vec3 Normal;
in float height;
in vec3 FragPos;

uniform float minHeight, maxHeight;
uniform vec3 viewPos;
uniform bool toggleFog;
uniform bool toggleAtmosphere;

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

vec4 lerp(vec4 a, vec4 b, float x) {
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

	vec4 snow = vec4(1.0f);
	vec4 soil = vec4(0.39,0.29,0.0,1.0f);
	vec4 rock = vec4(0.3, 0.3, 0.3, 1.0f);
	vec4 water = vec4(0.0,0.53,0.74,1.0f);
	vec4 grass = vec4(0.13,0.55,0.13,1.0f);

	float snow_alt = 0.8;
	float rock_alt = 0.4;
	float soil_alt = 0.4;
	float grass_alt = 0.0;

	//determine by altitude
	if (frac >= snow_alt)
		col = snow; //snow
	else if (rock_alt < frac )
		col = lerp( rock, snow, (frac-rock_alt) / (snow_alt - rock_alt)); //rock
	else if (soil_alt < frac)
		col = lerp(soil, rock, (frac - soil_alt) / (rock_alt - soil_alt));
	else if (frac > grass_alt)
		col = lerp(grass, rock, (frac - grass_alt) / (rock_alt - grass_alt) );
	else 
		col = water;

	//determine by slope
	float slope_threshold = 0.75;
	if (frac != 0) {
		if (param <= slope_threshold)
			col = lerp(rock, col, 1 - param);
		else
			col += 0.1 * lerp(rock, col, (param - slope_threshold) / (1 - slope_threshold));
	}

	  // Blinn-Phong lighting
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lighting = calculateDirLight(dir, norm, viewDir, col.rgb);

    vec4 result = vec4(lighting, 1.0);
	
	//fog
	float depth = linearizeDepth(gl_FragCoord.z) / far;

	if(toggleAtmosphere){
		float lambda = pow(2.71828, -2.0 * depth);
		result = lambda * result + (1 - lambda) * vec4(0.50, 0.50, 0.50, 1.0f);
	}
	if (toggleFog){
		vec4 depthVec4 = vec4(vec3(pow(depth, 0.9)), 1.0);
		result = result * (1 - depthVec4) + depthVec4;
	}

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
    
    // Specular (Blinn-Phong uses halfway vector)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0); 
    vec3 specular = light.specular * spec;
    
    return ambient + diffuse;
}