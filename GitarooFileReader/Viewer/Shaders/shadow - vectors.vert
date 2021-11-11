R"(#version 330 core
layout(location = 0) in mat4 aMatrix;

out VS_OUT
{
	vec4 forwardVector;
	vec4 rightVector;
} vs_out;

void main()
{
	mat3 rot = mat3(aMatrix);
	vs_out.forwardVector = vec4(rot * vec3(100, 0, 0), 0);
	vs_out.rightVector = vec4(rot * vec3(0, 0, 100), 0);

	gl_Position = aMatrix[3] + vec4(rot * vec3(0, 1, 0), 0) - .5 * (vs_out.forwardVector + vs_out.rightVector);
}
)"
