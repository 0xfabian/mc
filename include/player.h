#pragma once

#include <global.h>
#include <camera.h>
#include <blocks.h>
#include <raycast.h>
#include <hitbox.h>
#include <world.h>
#include <hotbar.h>
#include <inventory.h>
#include <vector>
#include <algorithm>
#include <gtx/rotate_vector.hpp>
#include <gtx/vector_angle.hpp>

using namespace glm;

struct Player
{
    dvec3 pos;
    dvec3 vel;

    bool grounded;

    Camera cam;

    Hitbox hitbox;

    Hotbar hotbar;
    Inventory inventory;

    HitInfo hit_info;

    Player() {}
    Player(const dvec3& _pos);

    void handle_keys();
    void handle_clicks();
    void move();

    void update();
    void draw();
    void destroy();
};