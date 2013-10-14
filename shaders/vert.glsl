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

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightDirection_cameraspace = vec4(V*vec4(vec3(-1, 1, 1),0)).xyz;

    // Normal of the the vertex, in camera space
    vec3 Normal_cameraspace = vec4(V*M*vec4(normal_in,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

    UV = UV_in;
    normal = normalize(Normal_cameraspace);
    directionalLight = normalize(LightDirection_cameraspace);
}
