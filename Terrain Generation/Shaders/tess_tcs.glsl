#version 460 core

layout (vertices = 4) out;

uniform mat4 model;
uniform mat4 view;

in vec2 TexCoord[];
out vec2 TextureCoord[];

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    int MIN_TESS_LEVEL = 40;
    int MAX_TESS_LEVEL = 60;
    float MIN_DISTANCE = 100.0f;
    float MAX_DISTANCE = 2000.0f;

    //view space coordinates of patch vertices
    vec4 viewPos00 = view * model * gl_in[0].gl_Position;
    vec4 viewPos01 = view * model * gl_in[1].gl_Position;
    vec4 viewPos10 = view * model * gl_in[2].gl_Position;
    vec4 viewPos11 = view * model * gl_in[3].gl_Position;

    //t parameter for each vertex
    float dist00 = clamp((abs(viewPos00.z) - MIN_DISTANCE)/ (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);
    float dist01 = clamp((abs(viewPos01.z) - MIN_DISTANCE)/ (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);
    float dist10 = clamp((abs(viewPos10.z) - MIN_DISTANCE)/ (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);
    float dist11 = clamp((abs(viewPos11.z) - MIN_DISTANCE)/ (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);

    float tessUp = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist10, dist11));
    float tessRight = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist01, dist11));
    float tessLeft = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist10, dist00));
    float tessDown = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist00, dist01));

	if (gl_InvocationID == 0)
    {
       
        // CORRECT MAPPING
        gl_TessLevelOuter[0] = tessDown;
        gl_TessLevelOuter[1] = tessRight;
        gl_TessLevelOuter[2] = tessUp;
        gl_TessLevelOuter[3] = tessLeft;

        // This logic is correct
        gl_TessLevelInner[0] = max(tessUp, tessDown);
        gl_TessLevelInner[1] = max(tessRight, tessLeft);

    }


}
