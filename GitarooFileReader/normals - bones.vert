#version 430 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 4) in int aEnvelope;
layout(location = 5) in vec4 aWeights;

const int MAX_ENVELOPES = 64;
const int MAX_INSTANCES = 32;
const int MAX_BONES = 4;

out VS_OUT
{
	vec3 normal;
} vs_out;

layout (std140) uniform View
{
	mat4 view;
};

struct Envelope
{
	int numBones;
	mat4 bones[MAX_INSTANCES][MAX_BONES];
};

layout (std430) buffer Envelopes
{
	Envelope envelopes[MAX_ENVELOPES];
};

uniform mat4 models[MAX_INSTANCES];
uniform mat3 normalMatrices[MAX_INSTANCES];

void main()
{
	vec4 finalPos = vec4(0.0);
	vec4 finalNorm = vec4(0.0);
	
	for (int i = 0; i < envelopes[aEnvelope].numBones; ++i)
	{
		finalPos += envelopes[aEnvelope].bones[gl_InstanceID][i] * vec4(aPos.xyz, 1) * aWeights[i];
		finalNorm += envelopes[aEnvelope].bones[gl_InstanceID][i] * vec4(aNorm.xyz, 0) * aWeights[i];
	}
		
	gl_Position = view * models[gl_InstanceID] * finalPos;
	vs_out.normal = normalize(normalMatrices[gl_InstanceID] * vec3(finalNorm));
}
