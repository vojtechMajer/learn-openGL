#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec3 my_color;
out vec3 o_color;

void main()
{
    o_color = my_color;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}