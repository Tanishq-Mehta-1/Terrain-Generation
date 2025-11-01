#version 460 core

in vec3 Normal;
in float height;
in vec3 FragPos;

uniform float minHeight, maxHeight;
uniform vec3 viewPos;
uniform vec4 bgCol;
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
    // Colors
    vec3 deep_water    = vec3(0.109, 0.239, 0.321);
    vec3 shallow_water = vec3(0.235, 0.513, 0.568);
    vec3 sand          = vec3(0.839, 0.745, 0.552);
    vec3 grass         = vec3(0.419, 0.580, 0.286);
    vec3 forest        = vec3(0.345, 0.298, 0.172);
    vec3 rock          = vec3(0.445, 0.429, 0.421);
    vec3 snow          = vec3(0.941, 0.956, 0.968);

    // Normalized height thresholds for blending
    float water_level  = 0.05;
    float sand_level   = 0.10;
    float grass_level  = 0.35;
    float forest_level = 0.60;
    float rock_level   = 0.75; 
    float snow_level   = 0.80; 

    // Normalize the current fragment's height to a 0.0 - 1.0 range
    float frac = (height - minHeight) / (maxHeight - minHeight);
    vec3 col = deep_water; // Start with the lowest color

    // Blend between colors up the gradient using smoothstep for soft transitions
    col = mix(col, shallow_water, smoothstep(0.0, water_level, frac));
    col = mix(col, sand,          smoothstep(water_level, sand_level, frac));
    col = mix(col, grass,         smoothstep(sand_level, grass_level, frac));
    col = mix(col, forest,        smoothstep(grass_level, forest_level, frac));
    col = mix(col, rock,          smoothstep(forest_level, rock_level, frac));
    col = mix(col, snow,          smoothstep(rock_level, snow_level, frac));

//    // Get the steepness of the terrain. 1.0 = flat, 0.0 = vertical cliff.
//    float slope = dot(normalize(Normal), vec3(0.0, 1.0, 0.0));
//    // It will be 0.0 for gentle slopes and 1.0 for steep cliffs.
//    float rock_factor = 1.0 - smoothstep(0.45, 0.75, slope);
//    if (frac > water_level) {
//        col = mix(col, rock, rock_factor);
//    }

	  // Blinn-Phong lighting
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lighting = calculateDirLight(dir, norm, viewDir, col.rgb);

    vec4 result = vec4(lighting, 1.0);
	
	//fog
	float depth = linearizeDepth(gl_FragCoord.z) / far;

	if(toggleAtmosphere){
		float lambda = pow(2.71828, -0.2 * depth);
		result = lambda * result + (1 - lambda) * bgCol;
	}
	if (toggleFog){
		vec4 depthVec4 = vec4(vec3(pow(depth, 1.0)), 1.0);
		result = result * (1 - depthVec4) + depthVec4;
	}

	//gamma correction
	//result = pow(result, vec4(1.0f/2.2f));

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