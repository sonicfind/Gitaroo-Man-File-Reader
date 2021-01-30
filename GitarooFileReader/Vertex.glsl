#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;

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
uniform int textEnv;

void main()
{
	vec4 finalPos = vec4(aPos.xyz, 1);
	gl_Position = projection * view * model * finalPos;
	vs_out.fragPos = vec3(model * finalPos);
	vs_out.normal = vec3(model * vec4(aNorm, 1));
	vs_out.color = aColor;
	if (textEnv == 0)
		vs_out.texCoord = aTexCoord;
	else
	{
		mat3 normalMatrix = mat3(transpose(inverse(view * model)));
		vec3 r = reflect(vec3(view * model * finalPos), normalMatrix * aNorm);
		float m = 2 * sqrt(pow(r.x, 2) + pow(r.y, 2) + pow(r.z + 1, 2));
		vs_out.texCoord = r.xy / m + .5;
	}
}
