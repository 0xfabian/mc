#pragma once

#include <glm.hpp>
#include <ext.hpp>

using namespace glm;

struct Camera
{
    vec3 pos;
    vec3 forward;
    vec3 up;

    float target_fov;
    float fov;
    float aspect_ratio;

    float near;
    float far;

    Camera() {}
    Camera(const vec3& _pos, float _fov, float _aspect_ratio, float _near, float _far);
    mat4 matrix(const vec3& offset);
};