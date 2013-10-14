#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 UV_in;
layout(location = 2) in vec3 normal;

uniform mat4 MVP;

out vec2 UV;

void main()
{
    vec4 v = vec4(position, 1.0);
    gl_Position = MVP*v;

    UV = UV_in;
}
