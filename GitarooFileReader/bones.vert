#version 430 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in int aEnvelope;
layout(location = 5) in vec4 aWeights;

const int MAX_ENVELOPES = 64;
const int MAX_BONES = 4;

out VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord[2];
	vec4 color;
} vs_out;

layout (std140) uniform View
{
	mat4 view;
};

layout (std140) uniform Projection
{
	mat4 projection;
};

struct Envelope
{
	int numBones;
	mat4 bones[MAX_BONES];
};

layout (std430) buffer Envelopes
{
	Envelope envelopes[MAX_ENVELOPES];
};

uniform mat4 model;
uniform mat3 normalMatrix;
uniform int textEnv[2];
uniform int doMulti;

void main()
{
	vec4 finalPos = vec4(0.0);
	vec4 finalNorm = vec4(0.0);
	
	for (int i = 0; i < envelopes[aEnvelope].numBones; ++i)
	{
		finalPos += envelopes[aEnvelope].bones[i] * vec4(aPos.xyz, 1) * aWeights[i];
		finalNorm += envelopes[aEnvelope].bones[i] * vec4(aNorm.xyz, 0) * aWeights[i];
	}
		
	gl_Position = projection * view * model * finalPos;
	vs_out.fragPos = vec3(model * finalPos);
	vs_out.normal = normalize(normalMatrix * vec3(finalNorm));
	vs_out.color = aColor;

	if (textEnv[0] == 0)
		vs_out.texCoord[0] = aTexCoord;
	else
	{
		vec3 r = reflect(vec3(view * model * finalPos), normalMatrix * vec3(finalNorm));
		float m = 2 * sqrt(pow(r.x, 2) + pow(r.y, 2) + pow(r.z + 1, 2));
		vs_out.texCoord[0] = r.xy / m + .5;
	}
	
	if (doMulti == 1)
	{
		if (textEnv[1] == textEnv[0])
			vs_out.texCoord[1] = vs_out.texCoord[0];
		else if (textEnv[1] == 0)
			vs_out.texCoord[1] = aTexCoord;
		else
		{
			vec3 r = reflect(vec3(view * model * finalPos), normalMatrix * vec3(finalNorm));
			float m = 2 * sqrt(pow(r.x, 2) + pow(r.y, 2) + pow(r.z + 1, 2));
			vs_out.texCoord[1] = r.xy / m + .5;
		}
	}
}
