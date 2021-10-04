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
	
	// If true, then the image can be taken as an alpha map where
	// all color channels are equal with the final color being pure white
	if (texColor.a == 1)
	{
		texColor.a = texColor.r;
		texColor.rgb = vec3(1);
	}
	else
	    texColor.a *= 2;
	
	texColor.a *= (fColors.r + fColors.g + fColors.b) / 3;


	FragColor = texColor;
}