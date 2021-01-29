#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 4) in vec4 bPos;
layout(location = 5) in vec3 bNorm;

out VS_OUT
{
	vec3 normal;
} vs_out;

layout (std140) uniform View
{
	mat4 view;
};

uniform mat4 model;

struct Interpolate
{
	int position;
	int normal;
};
uniform Interpolate interpolation;
uniform float coefficient;

void main()
{
	vec4 finalPos;
	if (interpolation.position > 0 && coefficient > 0.01)
		finalPos = mix(aPos, bPos, coefficient);
	else
		finalPos = aPos;
	finalPos.w = 1;
	gl_Position = view * model * finalPos;
	
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	if (interpolation.normal > 0 && coefficient > 0.01)
		vs_out.normal = normalize(vec3(vec4(normalMatrix * mix(aNorm, bNorm, coefficient), 0.0)));
	else
		vs_out.normal = normalize(vec3(vec4(normalMatrix * aNorm, 0.0)));
}