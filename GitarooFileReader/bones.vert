#version 430 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in int aEnvelope;
layout(location = 5) in vec4 aWeights;

const int MAX_ENVELOPES = 64;
const int MAX_INSTANCES = 32;
const int MAX_BONES = 4;

out VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
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
	mat4 bones[MAX_INSTANCES][MAX_BONES];
};

layout (std430) buffer Envelopes
{
	Envelope envelopes[MAX_ENVELOPES];
};

uniform mat4 models[MAX_INSTANCES];
uniform mat3 normalMatrices[MAX_INSTANCES];

layout (std140) uniform Material
{
	int blendType;
	int shadingType;
	vec4 diffuse;
	vec4 specular;
	int flags;
	int textEnv;
	int uTile;
	int vTile;
};

void main()
{
	const mat4 model = models[gl_InstanceID];
	const mat3 normalMatrix = normalMatrices[gl_InstanceID];
	vec4 finalPos = vec4(0.0);
	vec4 finalNorm = vec4(0.0);
	
	for (int i = 0; i < envelopes[aEnvelope].numBones; ++i)
	{
		finalPos += envelopes[aEnvelope].bones[gl_InstanceID][i] * vec4(aPos.xyz, 1) * aWeights[i];
		finalNorm += envelopes[aEnvelope].bones[gl_InstanceID][i] * vec4(aNorm.xyz, 0) * aWeights[i];
	}

	gl_Position = projection * view * model * finalPos;
	vs_out.fragPos = vec3(model * finalPos);
	vs_out.normal = normalize(normalMatrix * vec3(finalNorm));
	vs_out.color = aColor;

	if (textEnv == 0)
		vs_out.texCoord = aTexCoord;
	else
	{
		vec3 r = reflect(vec3(view * model * finalPos), normalMatrix * vec3(finalNorm));
		float m = 2 * sqrt(pow(r.x, 2) + pow(r.y, 2) + pow(r.z + 1, 2));
		vs_out.texCoord = r.xy / m + .5;
	}
}
