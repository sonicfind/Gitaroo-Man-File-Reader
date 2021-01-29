#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 4) in vec4 aWeights;
layout(location = 5) in ivec4 aBoneIDs;

const int MAX_WEIGHTS = 4;
const int MAX_BONES = 64;

out VS_OUT
{
	vec3 normal;
} vs_out;

layout (std140) uniform View
{
	mat4 view;
};

uniform mat4 model;

layout (std140) uniform Bones
{
	mat4 boneTransforms[MAX_BONES];
};

void main()
{
	vec4 finalPos = vec4(0.0);
	vec4 finalNorm = vec4(0.0);

	float totalWeight = 0;
	for (int i = 0; i < MAX_WEIGHTS && totalWeight < .99; ++i)
	{
		mat4 boneTransform = boneTransforms[aBoneIDs[i]];
		vec4 localPosition = boneTransform * aPos;
		finalPos += localPosition * aWeights[i];
	
		vec4 worldNormal = boneTransform * vec4(aNorm.xyz, 0);
		finalNorm += worldNormal * aWeights[i];
		totalWeight += aWeights[i];
	}
	finalPos.w = 1;
	
	gl_Position = view * model * finalPos;
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vs_out.normal = normalize(vec3(vec4(normalMatrix * vec3(finalNorm), 0.0)));
}