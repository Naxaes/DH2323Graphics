// Introduction lab that covers:
// * C++
// * SDL
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation
// * glm::vec3 and std::vector

#include "SDLhelper.h"
#include "TestModel.h"
#include "utilities.h"


struct Camera
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 right    = glm::vec3(1.0f, 0.0f,  0.0f);
    glm::vec3 up       = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 forward  = glm::vec3(0.0f, 0.0f, -1.0f);

    float yaw = 0;

    glm::mat3 cached_rotation_matrix = glm::mat3();  // Initialized as identity matrix.
};

struct Light
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 color    = glm::vec3(1.0f, 1.0f, 1.0f) * 14.0f;
    glm::vec3 ambient  = glm::vec3(1.0f, 1.0f, 1.0f) *  0.5f;
};

struct Intersection
{
    glm::vec3 position       = glm::vec3(0);
    float     distance       = std::numeric_limits<float>::max();
    int       triangle_index = -1;

    explicit operator bool() const noexcept { return triangle_index != -1; }
};


[[gnu::const]] inline
glm::mat3 RotationMatrixX(const float radians)
{
    return glm::mat3(
            glm::vec3(1,      0,           0     ),
            glm::vec3(0,  cos(radians), -sin(radians)),
            glm::vec3(0,  sin(radians),  cos(radians))
    );
}

[[gnu::const]] inline
glm::mat3 RotationMatrixY(const float radians)
{
    return glm::mat3(
            glm::vec3( cos(radians),  0,  sin(radians)),
            glm::vec3(    0,          1,       0      ),
            glm::vec3(-sin(radians),  0,  cos(radians))
    );
}

[[gnu::const]] inline
glm::mat3 RotationMatrixZ(const float radians)
{
    return glm::mat3(
            glm::vec3(cos(radians), -sin(radians),  0),
            glm::vec3(sin(radians),  cos(radians),  0),
            glm::vec3(      0,            0,        1)
    );
}


glm::vec3 DirectLight(const glm::vec3& point, const glm::vec3& normal, const glm::vec3& color, const Light& light)
{
    using namespace glm;

    const vec3  point_to_light = light.position - point;
    const vec3  direction_to_light = normalize(point_to_light);
    const float radius = length(point_to_light);

    const float factor = max(dot(direction_to_light, normal), 0.0f);

    const vec3 result = (color * light.color * factor) / (4.0f * PI * radius * radius);

    return result;
}


Intersection ClosestIntersection(const glm::vec3& start, const glm::vec3& direction, const std::vector<Triangle>& triangles)
{
    Intersection closest_intersection;

    for (int i = 0; i < triangles.size(); ++i)
    {
        const Triangle triangle = triangles[i];

        const glm::vec3 v0 = triangle.v0;
        const glm::vec3 v1 = triangle.v1;
        const glm::vec3 v2 = triangle.v2;

        const glm::vec3 e1 = v1 - v0;
        const glm::vec3 e2 = v2 - v0;

        const glm::vec3 b = start - v0;
        const glm::mat3 A ( -direction, e1, e2 );

        const glm::vec3 x = glm::inverse( A ) * b;

        const float t = x[0];
        const float u = x[1];
        const float v = x[2];

        // CLARIFY! u and v should be able to be 0, right?
        if (0 <= u and 0 <= v and (u + v) <= 1 and 0 <= t and t < closest_intersection.distance)
            closest_intersection = {start + t * direction, t, i};
    }

    return closest_intersection;
};


Array2D<Uint32> Draw(
        const Camera& camera, const Light& light,
        const int width, const int height,
        const float focal, const std::vector<Triangle>& triangles,
        const Uint32 background_color = ColorCode(GREY)
)
{
    using namespace glm;

    Array2D<Uint32> framebuffer(height, width);

    for (int row = 0; row < height; ++row)
    {
        for (int column = 0; column < width; ++column)
        {
            const vec3 direction = camera.cached_rotation_matrix * vec3(column - (width/2.0f), row - (height/2.0f), -focal);

            // Primary ray.
            const Intersection intersection = ClosestIntersection(camera.position, direction, triangles);
            if (!intersection)
            {
                framebuffer(row, column) = background_color;
                continue;
            }

            // Shadow ray.
            const vec3 intersection_to_light = light.position - intersection.position;
            const vec3 direction_to_light    = normalize(intersection_to_light);
            // Move a short distance away so it doesn't collide with itself.
            const vec3 start_position        = intersection.position + direction_to_light * 0.001f;

            const Intersection blocking_intersection = ClosestIntersection(start_position, intersection_to_light, triangles);

            const Triangle triangle = triangles[intersection.triangle_index];

            const float factor  = max(dot(direction_to_light, triangle.normal), 0.0f);
            const vec3  diffuse = triangle.color * light.ambient * factor;

            // If there is an object before we reach the light, don't calculate light.
            if (blocking_intersection.distance < length(intersection_to_light))
            {
                framebuffer(row, column) = ColorCode(diffuse);
            }
            else
            {
                const vec3 specular = DirectLight(intersection.position, triangle.normal, triangle.color, light);
                const vec3 color = clamp(diffuse + specular, vec3(0), vec3(1));
                framebuffer(row, column) = ColorCode(color);
            }
        }
    }

    return framebuffer;
}

bool UpdateCamera(Camera& camera, const Uint8* key_state, const float delta)
{
    const float camera_movement_speed = 5.0f * delta;
    const float camera_rotation_speed = 1.0f * delta;

    bool updated = false;

    if (key_state[SDL_SCANCODE_D])      { camera.position += camera.right   * camera_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_A])      { camera.position -= camera.right   * camera_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_W])      { camera.position += camera.forward * camera_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_S])      { camera.position -= camera.forward * camera_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_SPACE])  { camera.position += camera.up      * camera_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_LCTRL])  { camera.position -= camera.up      * camera_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_Q])      { camera.yaw -= camera_rotation_speed; updated = true; }
    if (key_state[SDL_SCANCODE_E])      { camera.yaw += camera_rotation_speed; updated = true; }

    if (updated)
    {
        const glm::mat3 rotation = RotationMatrixY(camera.yaw);
        camera.right   =  glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]);
        camera.up      = -glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]);
        camera.forward = -glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2]);

        camera.cached_rotation_matrix = rotation;
    }

    return updated;
}

bool UpdateLight(Light& light, const Uint8* key_state, const float delta)
{
    const float light_movement_speed = 2.0f * delta;

    bool updated = false;

    if (key_state[SDL_SCANCODE_D])      { light.position.x += light_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_A])      { light.position.x -= light_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_W])      { light.position.z -= light_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_S])      { light.position.z += light_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_SPACE])  { light.position.y -= light_movement_speed; updated = true; }
    if (key_state[SDL_SCANCODE_LCTRL])  { light.position.y += light_movement_speed; updated = true; }

    return updated;
}


int main()
{
    constexpr int width  = 300;
    constexpr int height = 300;

    constexpr float focal_length = width / 2.0f;

    InitializeSDL2();

    Window window = CreateWindow("Lab2", width, height);
    Array2D<Uint32> framebuffer(height, width);
    
    Camera camera;
    Clock  clock;
    Light  light;

    light.position  = glm::vec3(0.0f, 0.0f, 1.0f);
    camera.position = glm::vec3(0.0f, 0.0f, 2.0f);

    const std::vector<Triangle> model = LoadTestModel();

    bool running = true;
    unsigned frame = 0;
    bool needs_update = true;

    while (running)
    {
        float delta_time = Tick(clock);

        // --- HANDLE EVENTS ----
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE)
                    Clear(window, WHITE);
                if (event.key.keysym.sym == SDLK_p)
                    ScreenShot(window);
            }
        }


        // --- UPDATE ----
        const Uint8* key_state = SDL_GetKeyboardState(nullptr);

        if (not key_state[SDL_SCANCODE_LSHIFT])
            needs_update |= UpdateCamera(camera, key_state, delta_time);
        else
            needs_update |= UpdateLight(light,   key_state, delta_time);


        if (not needs_update)
            continue;
        needs_update = false;

        // --- RENDER ----
        printf(
            "Frame: %i | Position (%f, %f, %f) | Y-Rotation %f | Light (%f, %f, %f) | Delta %f\n",
            ++frame, camera.position.x, camera.position.y, camera.position.z, camera.yaw,
            light.position.x, light.position.y, light.position.z, delta_time
        );
        Array2D<Uint32> colors = Draw(camera, light, width, height, focal_length, model);
        FillWindow(window, colors.data);
        Render(window);

    }


    // ---- CLEAN UP ----
    DestroyWindow(window);
    SDL_Quit();
}