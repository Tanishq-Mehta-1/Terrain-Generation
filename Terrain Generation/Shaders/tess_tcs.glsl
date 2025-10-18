#version 460 core

layout (vertices = 4) out;

in vec2 TexCoord[];
out vec2 TextureCoord[];

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    int count  = 64;

	if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = count;
        gl_TessLevelOuter[1] = count;
        gl_TessLevelOuter[2] = count;
        gl_TessLevelOuter[3] = count;

        gl_TessLevelInner[0] = count;
        gl_TessLevelInner[1] = count;
    }


}
