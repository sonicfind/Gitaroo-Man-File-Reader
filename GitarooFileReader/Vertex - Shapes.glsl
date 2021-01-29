#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in vec4 bPos;
layout(location = 5) in vec3 bNorm;
layout(location = 6) in vec4 bColor;
layout(location = 7) in vec2 bTexCoord;

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

struct Interpolate
{
	int position;
	int normal;
	int color;
	int texCoord;
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
	
	gl_Position = projection * view * model * finalPos;
	vs_out.fragPos = vec3(model * finalPos);
	
	if (interpolation.normal > 0 && coefficient > 0.01)
		vs_out.normal = vec3(model * vec4(mix(aNorm, bNorm, coefficient), 1));
	else
		vs_out.normal = vec3(model * vec4(aNorm, 1));
	
	if (interpolation.color > 0 && coefficient > 0.01)
		vs_out.color = mix(aColor, bColor, coefficient);
	else
		vs_out.color = aColor;
	
	if (interpolation.texCoord > 0 && coefficient > 0.01)
		vs_out.texCoord = mix(aTexCoord, bTexCoord, coefficient);
	else
		vs_out.texCoord = aTexCoord;
}
