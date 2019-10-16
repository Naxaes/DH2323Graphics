#define CRASH_ON_ASSERT true

#include <glm/gtc/matrix_transform.hpp>

#include "SDLhelper.h"
#include "TestModel.h"
#include "interpolation.h"
#include "intersection.h"
#include "utilities.h"
#include "camera.h"

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float_t;
using f64 = double_t;


struct Light
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 color    = glm::vec3(1.0f, 1.0f, 1.0f) * 1.4f;
    glm::vec3 ambient  = glm::vec3(1.0f, 1.0f, 1.0f) * 0.5f;
};

struct Vertex
{
    glm::vec3 position = glm::vec3();

    Vertex() = default;
    explicit Vertex(const glm::vec3& position) : position(position) {}
};

using Viewport = AABB;


/*
// For convex polygon only.
std::vector<Pixel> ScanlineRasterize(const std::vector<Pixel>& vertices)
{
    constexpr int MAX_INT = std::numeric_limits<int>::max();
    constexpr int MIN_INT = std::numeric_limits<int>::min();

    // 1. Find max and min y-value of the polygon
    // and compute the number of rows it occupies.
    int bottom = MIN_INT;
    int top    = MAX_INT;
    for (const Pixel& vertex : vertices)
    {
        if (vertex.y > bottom)
            bottom = vertex.y;
        if (vertex.y < top)
            top = vertex.y;
    }

    // Break early if polygon is outside of the screen.
    // if (top >= viewport.bottom) return {};
    // if (bottom < viewport.top)  return {};

    // // Only care about the rows inside the viewport.
    // if (bottom >= viewport.bottom) bottom = viewport.bottom - 1;
    // if (top    <  viewport.top)    top    = viewport.top;

    Assert(bottom != MIN_INT and top != MAX_INT, "Top (%i) or bottom (%i) hasn't been initialized!", top, bottom);
    Assert(bottom >= top, "Min (%i) is greater than max (%i).", top, bottom);


    // 2. Resize leftPixels and rightPixels
    // so that they have an element for each row.
    const int rows = std::max(bottom - top + 1, 1024);

    std::vector<Pixel> left  (static_cast<unsigned long>(rows));
    std::vector<Pixel> right (static_cast<unsigned long>(rows));


    // 3. Initialize the x-coordinates in leftPixels
    // to some really large value and the x-coordinates
    // in rightPixels to some really small value.
    for (auto& pixel : left)
        pixel.x = MAX_INT;
    for (auto& pixel : right)
        pixel.x = MIN_INT;


    // 4. Loop through all edges of the polygon and use
    // linear interpolation to find the x-coordinate for
    // each row it occupies. Update the corresponding
    // values in rightPixels and leftPixels.
    for (unsigned i = 0; i < vertices.size(); ++i)
    {
        const Pixel& first = vertices[i];
        const Pixel& next  = vertices[(i+1) % vertices.size()];

        const std::vector<Pixel> line_fragments = Interpolate(first, next);

        for (const Pixel& line_fragment : line_fragments)
        {
            int row_index = line_fragment.y - top;

            if (not (0 <= row_index and row_index < rows))
                continue;

            if (left[row_index].x > line_fragment.x)
                left[row_index] = line_fragment;
            if (right[row_index].x < line_fragment.x)
                right[row_index] = line_fragment;
        }
    }


    std::vector<Pixel> pixels;
    pixels.reserve(static_cast<unsigned long>(rows));
    for (int row = 0; row < rows; ++row)
    {
        const std::vector<Pixel> row_pixels = Interpolate(left[row], right[row]);
        pixels.insert(end(pixels), begin(row_pixels), end(row_pixels));
    }

    return pixels;
}

*/


std::vector<Pixel> Rasterize(const Viewport& viewport, const glm::ivec3& p0, const glm::ivec3& p1, const glm::ivec3& p2)
{
    // TODO(ted): Guard-clipping and viewport-clipping.
    // https://fgiesen.wordpress.com/2011/07/05/a-trip-through-the-graphics-pipeline-2011-part-5/

    // https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/

    using std::min;
    using std::max;

    ivec2 v0 (p0);
    ivec2 v1 (p1);
    ivec2 v2 (p2);

    // Compute triangle bounding box
    AABB aabb = BoundingBox(v0, v1, v2);

    // Clip against screen bounds
    aabb.left   = max(aabb.left,   viewport.left);
    aabb.top    = max(aabb.top,    viewport.top );
    aabb.right  = min(aabb.right,  viewport.right  - 1);
    aabb.bottom = min(aabb.bottom, viewport.bottom - 1);

    const auto EdgeFunction = [](const ivec2& v0, const ivec2& v1, const ivec2& v2) -> i32
    {
        return (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
    };

    const i32 width  = (aabb.right - aabb.left);
    const i32 height = (aabb.bottom - aabb.top);

    std::vector<Pixel> pixels;
    pixels.reserve(static_cast<u32>(width * height));

    f32 area = EdgeFunction(v0, v1, v2);

    // Rasterize
    ivec2 point;
    for (point.y = aabb.top; point.y <= aabb.bottom; point.y++)
    {
        for (point.x = aabb.left; point.x <= aabb.right; point.x++)
        {
            // If point is on or inside all edges, render pixel.
            // Determine barycentric coordinates
            i32 w0 = EdgeFunction(v1, v2, point);
            i32 w1 = EdgeFunction(v2, v0, point);
            i32 w2 = EdgeFunction(v0, v1, point);

            if (w0 <= 0 && w1 <= 0 && w2 <= 0)
            {
                float z = 1 / (
                        (w0 / area) / p0.z +
                        (w1 / area) / p1.z +
                        (w2 / area) / p2.z
                    );
                pixels.emplace_back(point.x, point.y, z, glm::vec3());
            }
        }
    }

    return pixels;
}


// http://fabiensanglard.net/polygon_codec/
std::vector<glm::ivec3> VertexShader(const Viewport& viewport, const std::vector<Vertex>& vertices, const Camera& camera)
{
    using namespace glm;

    std::vector<glm::ivec3> rasters;

    // Dimensions of the produced image.
    const i32 image_width  = viewport.right  - viewport.left;
    const i32 image_height = viewport.bottom - viewport.top;

    // Assuming device ratio is the same as film ratio, for now.
    const f32 film_aspect_ratio   = camera.film_aperture_width / camera.film_aperture_height;
    const f32 device_aspect_ratio = image_width / static_cast<f32>(image_height);
    Assert(device_aspect_ratio - film_aspect_ratio < 0.001f, "They are different! (%f != %f)", device_aspect_ratio, film_aspect_ratio);

    // Image plane coordinates.
    const f32 image_plane_top    = ((camera.film_aperture_height / 2) / camera.focal_length) * camera.distance_to_canvas;
    const f32 image_plane_right  = ((camera.film_aperture_width  / 2) / camera.focal_length) * camera.distance_to_canvas;
    const f32 image_plane_bottom = -image_plane_top;
    const f32 image_plane_left   = -image_plane_right;

    // const f32 fov = 2 * 180 / PI * std::atan((camera.film_aperture_width / 2) / camera.focal_length);


    for (const Vertex& vertex : vertices)
    {
        // Homogeneous coordinate w is 1 (since we don't do perspective), so no need to divide.
        // Vertex world position in relation to the camera.
        const vec3 camera_space = camera.cached_rotation_matrix * (vertex.position - camera.position);

        // The vertex's location in camera space projected onto the image plane.
        const f32 screen_position_x = (camera_space.x / -camera_space.z) * camera.distance_to_canvas;
        const f32 screen_position_y = (camera_space.y / -camera_space.z) * camera.distance_to_canvas;

        // Normalize between [-1, 1]
        const f32 r = image_plane_right;
        const f32 l = image_plane_left;
        const f32 t = image_plane_top;
        const f32 b = image_plane_bottom;
        const f32 normalized_device_coordinate_x = (2 * screen_position_x) / (r - l) - (r + l) / (r - l);
        const f32 normalized_device_coordinate_y = (2 * screen_position_y) / (t - b) - (t + b) / (t - b);

        // Pixel coordinate on image (in raster space, the y is down, so invert direction).
        const f32 raster_x = ((normalized_device_coordinate_x + 1) / 2) * image_width;
        const f32 raster_y = ((normalized_device_coordinate_y + 1) / 2) * image_height;
        const f32 raster_z = -camera_space.z;

        rasters.emplace_back(raster_x, raster_y, raster_z);
    }

    return rasters;
}

glm::vec3 PixelShader(const Pixel& pixel, const Light& light, const glm::vec3& normal, const glm::vec3& color)
{
    using namespace glm;

    const vec3 world_position = pixel.inverted_world_position * pixel.z;

    // Reflectance
    const vec3 fragment_to_light  = light.position - world_position;
    const vec3 direction_to_light = normalize(fragment_to_light);
    const f32  radius = length(fragment_to_light);

    const f32 factor = max(dot(direction_to_light, normal), 0.0f);

    const vec3 specular = (factor * light.color) / (4.0f * PI * radius * radius);
    const vec3 illumination = /* reflectance */ vec3(1.0f) * (specular + light.ambient);
    const vec3 output_color = clamp(color * illumination, vec3(0), vec3(1));

    return color;
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
glm::mat4 PerspectiveMatrix(const float near, const float far, const float fov)
{
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix
    const f32 s = 1 / tan((fov / 2) * PI / 180);  // Scaling factor.
    const f32 a = -far / (far - near);            // Map to 0 factor.
    const f32 b = -(far * near) / (far - near);   // Map to 1 factor.

    return glm::mat4(
            glm::vec4(s, 0, 0, 0),
            glm::vec4(0, s, 0, 0),
            glm::vec4(0, 0, a, -1),
            glm::vec4(0, 0, b, 0)
    );
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

bool HandleEvents(Camera& camera, Light& light, float delta)
{
    bool needs_update = false;

    SDL_Event event;
    while (SDL_PollEvent(&event))
        if (event.type == SDL_QUIT)
            std::exit(0);

    const Uint8* key_state = SDL_GetKeyboardState(nullptr);

    if (not key_state[SDL_SCANCODE_LSHIFT])
        needs_update |= UpdateCamera(camera, key_state, delta);
    else
        needs_update |= UpdateLight(light,   key_state, delta);

    return needs_update;
}


int main()
{
    constexpr i32 width  = 400;
    constexpr i32 height = 400;

    Array2D<f32> z_buffer(height, width);
    Array2D<u32> framebuffer(height, width);

    InitializeSDL2();

    Window window = CreateWindow("Lab3", width, height);
    Viewport viewport {0, 0, width, height};

    Clock  clock;
    Light  light;
    Camera camera;

    light.position  = glm::vec3(0.0f, 0.0f, 1.0f);
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);


    const std::vector<Triangle> model = LoadTestModel();

    bool needs_update = false;
    bool running = true;
    while (running)
    {
        const f32 delta = Tick(clock);

        // --- HANDLE EVENTS ----

        needs_update = HandleEvents(camera, light, delta);


        // --- RENDER ----
        Clear(window);
        Clear(framebuffer);
        Fill(z_buffer, camera.far);


        for (const auto& triangle : model)
        {
            const Vertex v0(triangle.v0);
            const Vertex v1(triangle.v1);
            const Vertex v2(triangle.v2);

            const std::vector<glm::ivec3> rasters = VertexShader(viewport, {v0, v1, v2}, camera);
            const std::vector<Pixel>      pixels  = Rasterize(viewport, rasters[0], rasters[1], rasters[2]);

            for (const Pixel& pixel : pixels)
            {
                if (z_buffer(pixel.location.y, pixel.location.x) > pixel.z)
                {
                    z_buffer(pixel.location.y, pixel.location.x) = pixel.z;
                    framebuffer(pixel.location.y, pixel.location.x) = ColorCode(triangle.color);
                }
            }
        }



        printf(
            "Position (%f, %f, %f) | Y-Rotation %f\n",
            camera.position.x, camera.position.y, camera.position.z, camera.yaw
        );
        FillWindow(window, framebuffer.data);
        Render(window);

    }


    // ---- CLEAN UP ----
    DestroyWindow(window);
    SDL_Quit();
}