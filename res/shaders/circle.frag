#version 330 core

layout(location = 0) out vec4 frag_color;

uniform vec3 color;
uniform sampler2D tex;

in vec2 v_texCoord;

void main()
{
    frag_color = vec4(texture(tex, v_texCoord)) + vec4(color, 0);
};
