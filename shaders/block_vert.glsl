#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in int data;

out vec3 pos;
out float diffuse;
out vec4 light;
out float ao;
out vec2 uv;

uniform mat4 cam_mat;
uniform int global_light;

uniform sampler2D tex1;
uniform sampler2D tex2;

float diffuse_shades[6] = float[](0.6f, 0.6f, 1.0f, 0.4f, 0.8f, 0.8f);
vec2 uv_offsets[4] = vec2[](vec2(0.0f, 1.0f), vec2(1.0f, 1.0f), vec2(1.0f, 0.0f), vec2(0.0f, 0.0f));

vec4 get_light(int level)
{
    int sky_level = level & 0x0f;
    int block_level = (level >> 4) & 0x0f;

    vec2 point = vec2(block_level / 16.0f, sky_level / 16.0f);

    vec4 day_color = texture(tex1, point);
    vec4 night_color = texture(tex2, point);

    vec4 last = mix(night_color, day_color, global_light / 15.0f);

    float gamma = 1;

    last.x = pow(last.x, gamma);
    last.y = pow(last.y, gamma);
    last.z = pow(last.z, gamma);

    return last;
}   

float get_ao(int level)
{
    float max_ao = 0.35f;

    return mix(1.0f, max_ao, level / 3.0f);
}

void main()
{
    gl_Position = cam_mat * vec4(in_pos, 1.0f);

    pos = in_pos;

    int light_data = data & 0xff;
    int normal = (data >> 8) & 7;
    int ao_data = (data >> 11) & 3;

    light = get_light(light_data);
    diffuse = diffuse_shades[normal];
    ao = get_ao(ao_data);

    int vi = (data >> 13) & 3;
    int u = (data >> 15) & 0x1f;
    int v = (data >> 20) & 0x1f;

    uv = (vec2(u, v) + uv_offsets[vi]) / 32.0f;
}