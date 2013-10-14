#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 UV_in;
layout(location = 2) in vec3 normal_in;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec2 UV;
out vec3 normal;
out vec3 directionalLight;

void main()
{
    vec4 v = vec4(position, 1.0);
    gl_Position =  MVP * v;

    // Position of the vertex, in worldspace : M * position
    vec3 Position_worldspace = vec4(M * v).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPosition_cameraspace = vec4(V * M * v).xyz;
    vec3 EyeDirection_cameraspace = vec3(10,10,15) - position;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosition_cameraspace = vec4( V * vec4(vec3(1000, 1000, 1500),1)).xyz;
    vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    // Normal of the the vertex, in camera space
    vec3 Normal_cameraspace = vec4( V * M * vec4(normal_in,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

    UV = UV_in;
    normal = Normal_cameraspace;
    directionalLight = LightDirection_cameraspace;
}
