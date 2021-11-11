R"(#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in VS_OUT
{
	vec4 forwardVector;
	vec4 rightVector;
} gs_in[];

out vec2 fTexCoord;
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

	gl_Position = combo * gl_in[0].gl_Position;
	fTexCoord = vec2(0, 0);
	EmitVertex();

	gl_Position = combo * (gl_in[0].gl_Position + gs_in[0].forwardVector);
	fTexCoord = vec2(0, 1);
	EmitVertex();

	gl_Position = combo * (gl_in[0].gl_Position + gs_in[0].rightVector);
	fTexCoord = vec2(1, 0);
	EmitVertex();

	gl_Position = combo * (gl_in[0].gl_Position + gs_in[0].forwardVector + gs_in[0].rightVector);
	fTexCoord = vec2(1, 1);
	EmitVertex();

	EndPrimitive();
}
)"
