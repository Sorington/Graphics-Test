#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 UV_in;
layout(location = 2) in vec3 normal_in;
layout(location = 3) in vec3 tangent_in;
layout(location = 4) in vec3 bitangent_in;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 depthBiasMVP;

uniform vec3 dirLight_in;
uniform vec3 ptLightPos;

out vec2 UV;
out vec3 dirLight;
out vec3 ptLight;
out vec3 eyeDir;
out float dist;

void main()
{
    vec4 v = vec4(position, 1.0);
    gl_Position =  MVP * v;

    dist = distance(ptLightPos, position);
    mat4 MV = V*M;
    mat3 MV3x3 = mat3(MV[0].xyz, MV[1].xyz, MV[2].xyz);

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 dirLightDirection_camspace = vec4(V*vec4(dirLight_in, 0)).xyz;

    // Position of the vertex, in worldspace : M * position
    vec3 vertexPos_worldspace = vec4(M * vec4(position,1)).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPos_camspace = vec4(V*M*vec4(position,1)).xyz;
    vec3 eyeDir_camspace = vec3(0,0,0) - vertexPos_camspace;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 lightPos_camspace = vec4(V*vec4(ptLightPos,1)).xyz;
    vec3 lightDir_camspace = lightPos_camspace - vertexPos_camspace;


    // Normal of the the vertex, in camera space
    vec3 normal_camspace = vec4(V*M*vec4(normal_in,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
    vec3 tangent_camspace = MV3x3 * normalize(tangent_in);
    vec3 bitangent_camspace = MV3x3 * normalize(bitangent_in);

    // Let's compute the directions in tangent space
    mat3 TBN = transpose(mat3(tangent_camspace, bitangent_camspace, normal_camspace));
    vec3 lightDir_tangspace = TBN*lightDir_camspace;
    vec3 dirLightDirection_tangspace = TBN*dirLightDirection_camspace;
    vec3 eyeDir_tangspace = TBN*eyeDir_camspace;

    UV = UV_in;
    dirLight = dirLightDirection_tangspace;
    eyeDir = eyeDir_tangspace;
    ptLight = lightDir_tangspace;
}
