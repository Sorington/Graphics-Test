#version 330 core
out vec4 color;

in vec2 UV;
in vec3 normal;
in vec3 directionalLight;

uniform sampler2D textureSampler;

void main()
{
    vec3 materialDiffuseColor = texture2D(textureSampler, UV).rgb;
    vec3 materialAmbientColor = materialDiffuseColor*vec3(0.2,0.2,0.2);
    float ang = clamp(dot(normal, directionalLight), 0.0, 1.0);
    float lightPower = 2.0;
    vec3 lightColor = vec3(0.8, 0.6, 0.4);
    color = vec4(materialAmbientColor + materialDiffuseColor*lightColor*lightPower*ang, 1.0);
}
