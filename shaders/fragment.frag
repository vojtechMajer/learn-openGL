#version 330 core

out vec4 FragColor;
in vec2 texCoord;
in vec3 color;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, texCoord);
} 
