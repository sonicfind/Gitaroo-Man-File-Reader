R"(#version 330 core
in vec2 fTexCoord;
uniform sampler2D shadowTexture;

out vec4 FragColor;

void main()
{
	FragColor = texture(shadowTexture, fTexCoord);
	FragColor.a *= 2;
}
)"
