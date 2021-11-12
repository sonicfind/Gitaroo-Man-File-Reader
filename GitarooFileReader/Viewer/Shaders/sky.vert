R"(#version 330 core
layout(location = 0) in ivec2 aScreenSize;

out VS_OUT
{
	vec2 screenSize;
} vs_out;

void main()
{
	vs_out.screenSize = .5 * aScreenSize;
}
)"
