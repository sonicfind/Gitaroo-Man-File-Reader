R"(#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in VS_OUT
{
	vec2 screenSize;
} gs_in[];

out vec2 fTexCoord;
void main()
{
	gl_Position = vec4(-1, -1, 0, 1);
	fTexCoord = vec2(0, 1);
	EmitVertex();

	gl_Position = vec4(-1, 1, 0, 1);
	fTexCoord = vec2(0, 0);
	EmitVertex();

	gl_Position = vec4(1, -1, 0, 1);
	fTexCoord = vec2(1, 1);
	EmitVertex();

	gl_Position = vec4(1, 1, 0, 1);
	fTexCoord = vec2(1, 0);
	EmitVertex();

	EndPrimitive();
}
)"
