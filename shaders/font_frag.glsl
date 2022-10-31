#version 460 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D tex3;

void main()
{
    vec4 color = texture(tex3, uv);

    if(color.w < 0.1f)
        discard;

    FragColor = color;
}