#version 330 core
layout(location = 0) out vec4 color;

in vec2 UV;
in vec3 dirLight;
in vec3 ptLight;
in vec3 eyeDir;
in float dist;

uniform sampler2D textureSampler;
uniform sampler2D normalMapSampler;
uniform sampler2D specularMapSampler;

void main()
{
    vec3 normal = normalize(texture2D(normalMapSampler, UV).rgb*2.0 - 1.0);
    vec3 n = normal;
    vec3 dl = normalize(dirLight);
    vec3 l = normalize(ptLight);
    vec3 e = normalize(eyeDir);
    vec3 r = reflect(-l,n);

    vec3 materialDiffuseColor = texture2D(textureSampler, UV).rgb;
    vec3 materialAmbientColor = materialDiffuseColor*vec3(0.3,0.3,0.3);
    vec3 materialSpecularColor = texture2D(specularMapSampler, UV).rgb;

    float dirLightAngle = max(dot(n, dl), 0.0);
    float ptLightAngle = clamp(dot(n, l), 0, 1);
    float ptLightReflect = clamp(dot(e, r), 0, 1);

    float dirLightPower = 3.0;
    float ptLightPower = 1.0;

    vec3 dirLightColor = vec3(0.8, 0.6, 0.4);
    vec3 ptLightColor = vec3(1.0, 1.0, 0.0);

    color = vec4(materialAmbientColor + materialDiffuseColor*dirLightColor*dirLightPower*dirLightAngle +
                 materialDiffuseColor*ptLightColor*ptLightPower*ptLightAngle/(dist*dist) +
                 materialSpecularColor*ptLightColor*ptLightPower*pow(ptLightReflect, 5)/(dist*dist), 1.0);


}
