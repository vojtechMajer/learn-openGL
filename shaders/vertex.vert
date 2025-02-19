#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in  vec3 aColor;
out vec3 color;

uniform vec2 offset; 

void main()
{
    color = aColor;
    gl_Position = vec4(aPos.x - offset.x, aPos.y + offset.y, aPos.z, 1.0);
}