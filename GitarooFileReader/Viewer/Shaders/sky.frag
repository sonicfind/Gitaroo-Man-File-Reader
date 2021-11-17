R"(#version 330 core
in vec2 fTexCoord;
uniform sampler2D skyTexture;
out vec4 FragColor;

void main()
{
	FragColor = texture(skyTexture, fTexCoord);
}
)"
