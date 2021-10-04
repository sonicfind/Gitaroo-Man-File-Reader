#version 330 core
out vec4 FragColor;
in vec4 fColors;
void main()
{
    FragColor = fColors;
}