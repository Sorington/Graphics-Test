#version 330 core
out vec4 color;

in vec2 UV;

uniform sampler2D textureSampler;

void main()
{
    color = vec4(texture2D(textureSampler, UV).rgb, 1.0);
//    color = vec4(UV, 0.0, 1.0);
}
