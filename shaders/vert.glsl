#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 UV_in;
layout(location = 2) in vec3 normal_in;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

uniform vec3 dirLight_in;
uniform vec3 ptLightPos;

out vec2 UV;
out vec3 normal;
out vec3 dirLight;
out vec3 ptLight;
out vec3 eyeDir;
out float dist;

void main() {
    vec4 v = vec4(position, 1.0);
    gl_Position =  MVP * v;

    dist = distance(ptLightPos, position);

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 dirLightDirection_camspace = vec4(V*vec4(-dirLight_in, 0)).xyz;

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

    UV = UV_in;
    normal = normal_camspace;
    dirLight = dirLightDirection_camspace;
    eyeDir = eyeDir_camspace;
    ptLight = lightDir_camspace;
}
