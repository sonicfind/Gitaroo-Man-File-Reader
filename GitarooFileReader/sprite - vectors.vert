#version 330 core
layout(location = 0) in int aTextureIndex;
layout(location = 1) in vec3 aPos;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec2 aTexOffset;
layout(location = 4) in vec2 aSize;
layout(location = 5) in vec4 aColors;

layout (std140) uniform View
{
	mat4 view;
};

out VS_OUT
{
	vec4 rightVector;
	vec4 upVector;
} vs_out;

void main()
{
	vs_out.rightVector = vec4(aSize.x * view[0][0], aSize.x * view[1][0], aSize.x * view[2][0], 0);
	vs_out.upVector = vec4(aSize.y * view[0][1], aSize.y * view[1][1], aSize.y * view[2][1], 0);

	gl_Position = vec4(aPos.xyz, 1) - .5 * (vs_out.rightVector + vs_out.upVector);
}