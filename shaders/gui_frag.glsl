#version 460 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, uv);

    FragColor = color; 
}