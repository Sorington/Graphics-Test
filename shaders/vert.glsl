#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

void main()
{
    gl_Position.xyz = vec3(1, 1, 0);
    gl_Position.w = 1.0;
}
