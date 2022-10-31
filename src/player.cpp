#include <player.h>

bool ray_vs_aabb(const dvec3& origin, const dvec3& dir, const Hitbox& box, double& t_hit_near, dvec3& normal)
{
    dvec3 t_near = (box.pos - origin) / dir;
    dvec3 t_far = (box.pos + box.size - origin) / dir;

    if (std::isnan(t_far.y) || std::isnan(t_far.x))     return false;
    if (std::isnan(t_near.y) || std::isnan(t_near.x))   return false;
    if (std::isnan(t_far.y) || std::isnan(t_far.z))     return false;
    if (std::isnan(t_near.y) || std::isnan(t_near.z))   return false;
    if (std::isnan(t_far.z) || std::isnan(t_far.x))     return false;
    if (std::isnan(t_near.z) || std::isnan(t_near.x))   return false;

    if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);
    if (t_near.z > t_far.z) std::swap(t_near.z, t_far.z);

    if (t_near.x > t_far.y || t_near.y > t_far.x
        || t_near.y > t_far.z || t_near.z > t_far.y
        || t_near.x > t_far.z || t_near.z > t_far.x)
        return false;

    t_hit_near = max(t_near.x, t_near.y, t_near.z);
    double t_hit_far = min(t_far.x, t_far.y, t_far.z);

    if (t_hit_far < 0)
        return false;

    if (t_hit_near > t_hit_far)
        return false;

    if (t_near.x > t_near.y)
    {
        if (t_near.x > t_near.z)
            normal = dvec3(-sign(dir.x), 0, 0);
        else
            normal = dvec3(0, 0, -sign(dir.z));
    }
    else
    {
        if (t_near.y > t_near.z)
            normal = dvec3(0, -sign(dir.y), 0);
        else
            normal = dvec3(0, 0, -sign(dir.z));
    }

    return true;
}

bool swept_aabb(const dvec3& vel, const Hitbox& box1, const Hitbox& box2, double& t, dvec3& normal, double dt)
{
    if (vel == dvec3(0))
        return false;

    Hitbox expanded;
    expanded.pos = box2.pos - box1.size / 2.0;
    expanded.size = box2.size + box1.size;

    if (ray_vs_aabb(box1.pos + box1.size / 2.0, vel * dt, expanded, t, normal))
        return t < 1.0;

    return false;
}

Player::Player(const dvec3& _pos)
{
    pos = _pos;
    vel = dvec3(0);

    grounded = false;

    cam = Camera(vec3(0, 1.62, 0), 80, win_size.x / (float)win_size.y, 0.1f, 1000.0f);

    hitbox = Hitbox(dvec3(-0.3, 0, -0.3), dvec3(0.6, 1.8, 0.6));

    hit_info.hit = false;

    inventory.set_item(0, 0, STONE);
    inventory.set_item(0, 1, DIRT);
    inventory.set_item(0, 2, GRASS_BLOCK);
    inventory.set_item(0, 3, GRASS);
    inventory.set_item(0, 4, LEAVES);
    inventory.set_item(0, 5, LOG);
    inventory.set_item(0, 6, PLANKS);
    inventory.set_item(0, 7, GLASS);
    inventory.set_item(0, 8, TORCH);
}

vec3 get_input()
{
    vec3 input = vec3(0);

    if (is_key_pressed(SDLK_w))         input += vec3(0, 0, 1);
    if (is_key_pressed(SDLK_s))         input += vec3(0, 0, -1);
    if (is_key_pressed(SDLK_a))         input += vec3(-1, 0, 0);
    if (is_key_pressed(SDLK_d))         input += vec3(1, 0, 0);
    if (is_key_pressed(SDLK_LCTRL))     input += vec3(0, -1, 0);
    if (is_key_pressed(SDLK_SPACE))     input += vec3(0, 1, 0);

    return input;
}

void Player::handle_keys()
{
    for (int i = 0; i < 9; i++)
        if (is_key_down(SDLK_1 + i))
            hotbar.set_index(i);

    if (is_key_down(SDLK_q))
        hotbar.set_item(-1);
}

void Player::handle_clicks()
{
    if (show_inventory)
    {
        if (inventory.over != NULL && is_button_down(0))
        {
            if (!inventory.over->is_empty())
                hotbar.set_item(inventory.over->get_item());
        }
    }
    else
    {
        vec3 ray_origin = (vec3)pos + cam.pos;
        vec3 ray_direction = cam.forward;

        Chunk* chunk = world->get_chunk_from_pos(ray_origin);

        hit_info = raycast(ray_origin, ray_direction, 4.5f, chunk);

        if (hit_info.hit)
        {
            if (is_button_down(0))
                world->remove_block(hit_info.chunk, hit_info.block_pos);

            if (is_button_down(1))
            {
                int id = hit_info.chunk->get(hit_info.block_pos)->id;

                int i = hotbar.find_item(id);

                if (i != -1)
                    hotbar.set_index(i);
                else
                {
                    i = hotbar.find_empty();

                    if (i != -1)
                        hotbar.set_index(i);

                    hotbar.set_item(id);
                }
            }

            if (is_button_down(2))
            {
                if (!hotbar.is_empty())
                {
                    ivec3 new_pos = hit_info.block_pos + hit_info.face_normal;

                    BlockState bs = { (char)hotbar.get_item(), 0, 0 };

                    bool failed = false;

                    if (bs.id == BlockID::LOG)
                    {
                        if (hit_info.face_normal.x != 0)
                            bs.data1 = 0;
                        else if (hit_info.face_normal.y != 0)
                            bs.data1 = 1;
                        else
                            bs.data1 = 2;
                    }
                    else if (bs.id == BlockID::TORCH)
                    {
                        if (hit_info.face_normal.y == 1)         bs.data1 = 0;
                        else if (hit_info.face_normal.z == 1)    bs.data1 = 1;
                        else if (hit_info.face_normal.z == -1)   bs.data1 = 2;
                        else if (hit_info.face_normal.x == 1)    bs.data1 = 3;
                        else if (hit_info.face_normal.x == -1)   bs.data1 = 4;
                        else                                    failed = true;
                    }

                    if (!failed)
                    {
                        Hitbox cube = Hitbox((dvec3)new_pos + chunk_pos_to_world_hp(hit_info.chunk->pos) - pos, dvec3(1));

                        if (!intersect(cube, hitbox))
                            world->place_block(hit_info.chunk, new_pos, &bs);
                    }
                }
            }
        }
    }
}

void Player::move()
{
    if (show_inventory)
        return;

    vec3 right = normalize(cross(cam.up, cam.forward));

    if (mouse_control)
    {
        ivec2 mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);

        float sens = 0.07f;
        float rot_x = sens * (mouse.x - win_size.x / 2.0f);
        float rot_y = sens * (mouse.y - win_size.y / 2.0f);

        SDL_WarpMouseInWindow(win, win_size.x / 2, win_size.y / 2);

        vec3 new_forward = rotate(cam.forward, radians(rot_y), right);

        if (dot(right, cross(cam.up, new_forward)) > 0)
            cam.forward = new_forward;

        cam.forward = rotate(cam.forward, radians(rot_x), cam.up);
    }

    vec3 input = get_input();

    if (!survival_mode)
    {
        if (length(input) > 0)
        {
            dvec3 norm_input = normalize((dvec3)input);

            double speed = is_key_pressed(SDLK_LSHIFT) ? 100 : 7;

            pos += norm_input.x * (dvec3)right * speed * delta_time;
            pos += norm_input.y * (dvec3)cam.up * speed * delta_time;
            pos += norm_input.z * (dvec3)cam.forward * speed * delta_time;
        }
    }
    else
    {
        double mass = 1;
        dvec3 force = dvec3(0);

        dvec3 input_xz = normalize(dvec3(input.x, 0, input.z));

        if (length(input_xz) > 0)
        {
            dvec3 forward = normalize(dvec3(cam.forward.x, 0, cam.forward.z));

            double f = 70;

            if (is_key_pressed(SDLK_LSHIFT))
                f = 140;

            force += (dvec3)right * input_xz.x * f;
            force += forward * input_xz.z * f;
        }

        if (is_key_down(SDLK_SPACE) && grounded)
        {
            vel += dvec3(0, 1, 0) * 8.0;
            grounded = false;
        }

        force += -vel * dvec3(20, 0, 20);
        force += dvec3(0, -1, 0) * mass * 25.0;

        dvec3 accel = force / mass;
        vel += accel * delta_time;

        Hitbox next_hitbox = hitbox;
        next_hitbox.pos += vel * delta_time;

        Hitbox bounds = get_bounds(hitbox, next_hitbox);
        bounds.pos += pos;

        Chunk* chunk = world->get_chunk_from_pos(bounds.pos);

        if (chunk == NULL)
            printf("chunk is null\n");

        dvec3 bpc = bounds.pos - chunk_pos_to_world_hp(chunk->pos);

        ivec3 start = bpc;
        ivec3 end = bpc + bounds.size;

        std::vector<Hitbox> collisions;

        for (int x = start.x; x <= end.x; x++)
            for (int y = start.y; y <= end.y; y++)
                for (int z = start.z; z <= end.z; z++)
                {
                    if (y < 0 || y >= CHUNK_HEIGHT)
                        continue;

                    ivec3 p = ivec3(x, y, z);

                    Chunk* _chunk = get_neighbor_chunk(chunk, p);

                    if (_chunk == NULL)
                        continue;

                    if (_chunk->get(p)->id != AIR)
                        collisions.push_back(Hitbox((dvec3)p + chunk_pos_to_world_hp(_chunk->pos) - pos, dvec3(1)));
                }

        double t;
        dvec3 normal;

        std::vector<std::pair<int, double>> order;

        for (int i = 0; i < collisions.size(); i++)
        {
            if (swept_aabb(vel, hitbox, collisions[i], t, normal, delta_time))
                order.push_back({ i, t });
        }

        std::sort(order.begin(), order.end(),
            [](const std::pair<int, double>& a, const std::pair<int, double>& b)
            {
                return a.second < b.second;
            });


        for (int i = 0; i < order.size(); i++)
        {
            if (swept_aabb(vel, hitbox, collisions[order[i].first], t, normal, delta_time))
            {
                if (t >= 0)
                {
                    vel += -(vel * normal) * normal * (1 - t);
                    //printf("t = %lf          %lf %lf %lf\n", t, collisions[order[i].first].pos.x, collisions[order[i].first].pos.y, collisions[order[i].first].pos.z);
                }

                if (normal == dvec3(1, 0, 0) || normal == dvec3(-1, 0, 0))
                    vel.x = 0;

                if (normal == dvec3(0, 1, 0))
                {
                    vel.y = 0;
                    grounded = true;
                }

                if (normal == dvec3(0, -1, 0))
                    vel.y = 0;

                if (normal == dvec3(0, 0, 1) || normal == dvec3(0, 0, -1))
                    vel.z = 0;

                //if (t >= 0)
                    //printf("vel = %lf %lf %lf\nnormal = %lf %lf %lf\n\n", vel.x, vel.y, vel.z, normal.x, normal.y, normal.z);
            }
        }

        pos += vel * delta_time;
    }
}

void Player::update()
{
    handle_keys();
    handle_clicks();
    move();
}

void Player::draw()
{
    mat4 cam_mat = cam.matrix((vec3)pos);
    line_shader.bind();
    line_shader.upload_mat4("cam_mat", cam_mat);

    if (hit_info.hit)
    {
        line_shader.upload_vec4("color", vec4(0, 0, 0, 0.5f));

        Hitbox block_hitbox;

        block_hitbox.pos = (dvec3)hit_info.block_pos + chunk_pos_to_world_hp(hit_info.chunk->pos) - (dvec3)cam.forward * 0.005;
        block_hitbox.size = dvec3(1);

        block_hitbox.draw();
    }

    if (show_debug)
    {
        line_shader.upload_vec4("color", vec4(0, 0, 1, 1));

        Hitbox _hitbox = hitbox;
        _hitbox.pos += pos;
        _hitbox.draw();
    }
}

void Player::destroy()
{
    delete this;
}