#version 330 core
flat in int fTextureIndex;
in vec2 fTexCoord;
in vec4 fColors;
flat in int fBlendType;

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
	
	switch (fBlendType)
	{
	case 2:
		FragColor = fColors;
		break;
	case 3:
		FragColor = vec4(texColor.rgb, 1 - (1 - texColor.r) * (1 - texColor.g) * (1 - texColor.b));
		break;
	case 5:
		FragColor = vec4(texColor.rgb, 1);
		break;
	case 4:
		FragColor = vec4(texColor.rgb * .5, 1);
		break;
	default:
		FragColor = texColor * fColors;
	}
}