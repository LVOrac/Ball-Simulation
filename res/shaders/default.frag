#version 330 core

layout(location = 0) out vec4 frag_color;

uniform vec3 color;

void main()
{
    frag_color = vec4(color, 1);
};
