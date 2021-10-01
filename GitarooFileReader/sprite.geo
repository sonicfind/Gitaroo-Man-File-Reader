#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in VS_OUT
{
	int textureIndex;
	vec2 texCoord;
	vec2 texOffset;
	vec4 colors;
	vec4 rightVector;
	vec4 upVector;
} gs_in[];

flat out int fTextureIndex;
out vec2 fTexCoord;
out vec4 fColors;

layout (std140) uniform View
{
	mat4 view;
};

layout (std140) uniform Projection
{
	mat4 projection;
};

void main()
{
	mat4 combo = projection * view;
	fTextureIndex = gs_in[0].textureIndex;
	fColors = gs_in[0].colors;

	gl_Position = combo * gl_in[0].gl_Position;
	fTexCoord = gs_in[0].texCoord;
	EmitVertex();

	gl_Position = combo * (gl_in[0].gl_Position + gs_in[0].rightVector);
	fTexCoord = gs_in[0].texCoord + vec2(gs_in[0].texOffset.x, 0);
	EmitVertex();

	gl_Position = combo * (gl_in[0].gl_Position + gs_in[0].upVector);
	fTexCoord = gs_in[0].texCoord + vec2(0, gs_in[0].texOffset.y);
	EmitVertex();

	gl_Position = combo * (gl_in[0].gl_Position + gs_in[0].rightVector + gs_in[0].upVector);
	fTexCoord = gs_in[0].texCoord + gs_in[0].texOffset;
	EmitVertex();

	EndPrimitive();
}