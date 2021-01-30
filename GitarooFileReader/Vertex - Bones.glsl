#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in vec4 aWeights;
layout(location = 5) in ivec4 aBoneIDs;


const int MAX_WEIGHTS = 4;
const int MAX_BONES = 64;

out VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec4 color;
} vs_out;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

layout (std140) uniform Bones
{
	mat4 boneTransforms[MAX_BONES];
};

uniform int textEnv;

void main()
{
	vec4 finalPos = vec4(0.0);
	vec4 finalNorm = vec4(0.0);

	float totalWeight = 0;
	for (int i = 0; i < MAX_WEIGHTS && totalWeight < .99; ++i)
	{
		mat4 boneTransform = boneTransforms[aBoneIDs[i]];
		vec4 localPosition = boneTransform * vec4(aPos.xyz, 1);
		finalPos += localPosition * aWeights[i];
	
		vec4 worldNormal = boneTransform * vec4(aNorm.xyz, 0);
		finalNorm += worldNormal * aWeights[i];
		totalWeight += aWeights[i];
	}
	
	gl_Position = projection * view * model * finalPos;
	vs_out.fragPos = vec3(model * finalPos);
	vs_out.normal = vec3(model * finalNorm);
	vs_out.color = aColor;
	if (textEnv == 0)
		vs_out.texCoord = aTexCoord;
	else
	{
		mat3 normalMatrix = mat3(transpose(inverse(view * model)));
		vec3 r = reflect(vec3(view * model * finalPos), normalMatrix * vec3(finalNorm));
		float m = 2 * sqrt(pow(r.x, 2) + pow(r.y, 2) + pow(r.z + 1, 2));
		vs_out.texCoord = r.xy / m + .5;
	}
}
