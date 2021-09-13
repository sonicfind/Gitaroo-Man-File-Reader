#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;

const int MAX_INSTANCES = 32;

out VS_OUT
{
	vec3 normal;
} vs_out;

layout (std140) uniform View
{
	mat4 view;
};

uniform mat4 models[MAX_INSTANCES];
uniform mat3 normalMatrices[MAX_INSTANCES];

void main()
{
	gl_Position = view * models[gl_InstanceID] * vec4(aPos.xyz, 1);
	vs_out.normal = normalize(normalMatrices[gl_InstanceID] * aNorm);
}