#version 330 core
out vec4 color;

in vec2 UV;
in vec3 normal;
in vec3 directionalLight;

uniform sampler2D textureSampler;

void main()
{
    vec3 materialDiffuseColor = texture2D(textureSampler, UV).rgb;
    float ang = clamp(dot(normal, directionalLight), 0, 1);
    float lightPower = 1.0;
    vec3 lightColor = vec3(0.6, 0.4, 0.4);
    color = vec4(materialDiffuseColor*lightColor*lightPower*ang, 1.0);
}
