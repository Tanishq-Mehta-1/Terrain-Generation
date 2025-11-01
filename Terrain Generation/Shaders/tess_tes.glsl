#version 460 core

layout (quads, fractional_odd_spacing, ccw) in;

in vec2 TextureCoord[];

uniform sampler2D heightMap;
uniform float yScale;
uniform float yShift;
uniform float seaLevel;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float height;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    //current vertex uv
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

    //corner point's uv
	vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

	vec2 t0 = (t01 - t00)*u + t00;
    vec2 t1 = (t11 - t10)*u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    vec4 sampled_point = texture(heightMap, texCoord);
    height = (sampled_point.x )* yScale - yShift;

    //corner point's position
	vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 flat_normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    p += flat_normal * height;
    p.y = max(p.y, seaLevel);

    vec3 normal = flat_normal.xyz;

    if (p.y != seaLevel) {

        //calculating surface normals
        float hL, hR, hU, hD;
	    hL = hR = hD = hU = height;
        float offset = 10.0f;

        vec2 map_size = textureSize(heightMap, 0);
        vec2 dx = vec2(1.0f / float(map_size.x), 0.0f);
	    vec2 dy = vec2(0.0f, 1.0f / float(map_size.y));

        hL = texture(heightMap, texCoord  - dx).r * yScale - yShift;
        hR = texture(heightMap, texCoord  + dx).r * yScale - yShift;
        hD = texture(heightMap, texCoord  - dy).r * yScale - yShift;
        hU = texture(heightMap, texCoord  + dy).r * yScale - yShift;;

        normal = vec3((hL - hR) / 2.0f, 1.0f, (hD - hU) / 2.0f);
	    normal = normalize(normal);
    }
   

    gl_Position = projection * view * model * p;
    FragPos = vec3(model * gl_Position);
    Normal = mat3(transpose(inverse(model))) * normal;
}