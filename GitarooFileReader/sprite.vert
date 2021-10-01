#version 330 core
layout(location = 0) in int aTextureIndex;
layout(location = 1) in vec3 aPos;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec2 aTexOffset;
layout(location = 4) in vec2 aSize;
layout(location = 5) in vec4 aColors;

layout (std140) uniform SpriteVectors
{
	vec3 rightVector;
	vec3 upVector;
};

out VS_OUT
{
	int textureIndex;
	vec2 texCoord;
	vec2 texOffset;
	vec4 colors;
	vec4 rightVector;
	vec4 upVector;
} vs_out;

void main()
{
	vec4 finalPos = vec4(aPos.xyz, 1);
	finalPos.xy -= .5 * aSize;
	gl_Position = finalPos;
	vs_out.textureIndex = aTextureIndex;
	vs_out.texCoord = aTexCoord;
	vs_out.texOffset = aTexOffset;
	vs_out.colors = aColors;
	vs_out.rightVector = vec4(aSize.x * rightVector, 0);
	vs_out.upVector = vec4(aSize.y * upVector, 0);
}