#version 330 core
flat in int fTextureIndex;
in vec2 fTexCoord;
in vec4 fColors;

out vec4 FragColor;

uniform sampler2D textures[8];
layout (std140) uniform SpriteSizes
{
    ivec2 sizes[8];
};

void main()
{
    vec4 texColor = texture(textures[fTextureIndex], fTexCoord / sizes[fTextureIndex]);

    if (texColor.a == 1)
         texColor.a = (texColor.r + texColor.g + texColor.b) / 3;

    texColor.a *= 2;
    FragColor = texColor * fColors;

    if (FragColor.a < .01)
		discard;
}