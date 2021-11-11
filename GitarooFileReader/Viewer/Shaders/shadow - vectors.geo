R"(#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 4) out;
in VS_OUT
{
	vec4 forwardVector;
	vec4 rightVector;
} gs_in[];

layout (std140) uniform View
{
	mat4 view;
};

layout (std140) uniform Projection
{
	mat4 projection;
};

out vec4 fColors;

void makeLine(vec4 vector, vec4 color)
{
	mat4 combo = projection * view;
	gl_Position = combo * gl_in[0].gl_Position;
	fColors = color;
	EmitVertex();

	gl_Position = combo * (gl_in[0].gl_Position + vector);
	fColors = color;
	EmitVertex();

	EndPrimitive();
}

void main()
{
	makeLine(gs_in[0].forwardVector, vec4(1.0, 0.0, 0.0, 1.0));
	makeLine(gs_in[0].rightVector, vec4(0.0, 1.0, 0.0, 1.0));
}
)"
