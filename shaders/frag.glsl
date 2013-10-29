#version 330 core
layout(location = 0) out vec4 color;

in vec2 UV;
in vec3 normal;
in vec3 dirLight;
in vec3 ptLight;
in vec3 eyeDir;
in float dist;
in vec4 shadowCoord;

uniform sampler2D textureSampler;
uniform sampler2D specularMapSampler;
uniform sampler2D shadowMap;

vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

float random(vec3 seed, int i){
        vec4 seed4 = vec4(seed,i);
        float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
        return fract(sin(dot_product) * 43758.5453);
}

void main()
{
    vec3 n = normalize(normal);
    vec3 dl = normalize(dirLight);
    vec3 l = normalize(ptLight);
    vec3 e = normalize(eyeDir);
    vec3 r = reflect(-l,n);

    vec3 materialDiffuseColor = texture2D(textureSampler, UV).rgb;
    vec3 materialAmbientColor = materialDiffuseColor*vec3(0.2,0.2,0.2);
    vec3 materialSpecularColor = texture2D(specularMapSampler, UV).rgb;

    float dirLightAngle = max(dot(n, dl), 0.0);
    float ptLightAngle = clamp(dot(n, l), 0, 1);
    float ptLightReflect = clamp(dot(e, r), 0, 1);

    float bias = 0.005*tan(acos(dirLightAngle));
    bias = clamp(bias, 0, 0.01);

    float visibility = 1.0;
    for (int i=0;i<4;i++){
        int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
        if ( texture2D( shadowMap, shadowCoord.xy + poissonDisk[index]/700.0 ).z  <  shadowCoord.z-bias ){
            visibility-=0.2;
          }
    }

    float dirLightPower = 1.0;
    float ptLightPower = 0.0;

    vec3 dirLightColor = vec3(0.8, 0.6, 0.4);
    vec3 ptLightColor = vec3(1.0, 1.0, 0.0);

    color = vec4(materialAmbientColor + visibility * materialDiffuseColor*dirLightColor*dirLightPower*dirLightAngle +
                 materialDiffuseColor*ptLightColor*ptLightPower*ptLightAngle/(dist*dist) +
                 materialSpecularColor*ptLightColor*ptLightPower*pow(ptLightReflect, 5)/(dist*dist), 1.0);

    //color = vec4(shadowCoord.z, shadowCoord.z, shadowCoord.z, 0);
    //color = texture2D(shadowMap, shadowCoord.xy);
    //color = vec4(visibility, visibility, visibility, 0);

}
