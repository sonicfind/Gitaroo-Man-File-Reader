R"(#version 330 core
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

layout (std140) uniform Models
{
	mat4 models[MAX_INSTANCES];
};

void main()
{
	mat4 combo = view * models[gl_InstanceID];
	// Position relative to the camera
	gl_Position = combo * vec4(aPos.xyz, 1);

	// Normal vector relative to the camera
	vs_out.normal = normalize(vec3(combo * vec4(aNorm, 0)));
}
)"
