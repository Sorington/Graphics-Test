#version 330 core
in vec2 UV;

out vec3 color;

uniform sampler2D renderTexture;

void main(void)
{
    color = texture2D(renderTexture, UV).xyz;
}
