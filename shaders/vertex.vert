#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
uniform mat4 translation_mat;
out vec2 texCoord;
out vec3 color;


void main()
{
    color = aColor;
    texCoord = aTexCoord;
    gl_Position = translation_mat*vec4(aPos, 1);
}