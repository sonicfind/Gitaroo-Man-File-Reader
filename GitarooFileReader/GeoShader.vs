#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;

out VS_OUT
{
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = view * model * vec4(aPos.xyz, 1);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNorm, 0.0)));
}