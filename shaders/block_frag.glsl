#version 460 core

in vec3 pos;
in float diffuse;
in vec4 light;
in float ao;
in vec2 uv;

out vec4 FragColor;

uniform sampler2D tex0;

uniform vec3 eye_pos;
uniform vec4 fog_color;
uniform int show_ao;

float fog_factor(float d)
{
    float fog_max = 20.0f * 16.0f;
    float fog_min = 14.0f * 16.0f;

    if (d >= fog_max) return 1;
    if (d <= fog_min) return 0;

    return 1 - (fog_max - d) / (fog_max - fog_min);
}

void main()
{   
    float fog_effect = fog_factor(distance(eye_pos, pos));

    //vec4 color = vec4(1);
    vec4 color = texture(tex0, uv);

    if(color.w < 0.1f)
        discard;

    color *= diffuse * light;

    if(show_ao > 0)
        color *= ao;

    color.w = 1.0f;

    //FragColor = color;

    FragColor = mix(color, fog_color, fog_effect);
}