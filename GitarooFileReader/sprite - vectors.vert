#version 330 core
layout(location = 0) in int aTextureIndex;
layout(location = 1) in vec3 aPos;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec2 aTexOffset;
layout(location = 4) in vec2 aSize;
layout(location = 5) in vec4 aColors;

layout (std140) uniform CamPosition
{
	vec3 camPosition;
};

out VS_OUT
{
	vec4 rightVector;
	vec4 upVector;
} vs_out;

void main()
{
	vec3 camToPos = normalize(aPos - camPosition);
	vs_out.rightVector = vec4(aSize.x * camToPos.z, 0, aSize.x * -camToPos.x, 0);
	vs_out.upVector = vec4(0, aSize.y, 0, 0);

	gl_Position = vec4(aPos.xyz, 1) - .5 * (vs_out.rightVector + vs_out.upVector);
}