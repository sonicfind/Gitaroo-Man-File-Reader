#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 normal;
out vec4 ourColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.xyz, 1);
    fragPos = vec3(model * aPos);
    //normal = -aNorm;
    normal = vec3(aNorm.xy, -aNorm.z);
    ourColor = aColor;
    texCoord = aTexCoord;
}