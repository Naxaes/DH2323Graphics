#pragma once

#include <glm/glm.hpp>

#include "intersection.h"


struct Camera
{
    // Aperture/eye of the camera.
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    // The camera's local coordinate system in relation to the world.
    glm::vec3 right    = glm::vec3(1.0f, 0.0f,  0.0f);
    glm::vec3 up       = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 forward  = glm::vec3(0.0f, 0.0f, -1.0f);

    // Current rotation in y-axis.
    float yaw = 0;

    // Clipping distances of the virtual camera.
    float near = 1.0f;
    float far  = 1000.0f;


    // FOV, focal length and canvas size are dependent on each other. In other words, two of them describe the third.

    // Angle/Field of view. Higher values indicate greater field of vision (zooming out), while lower is zooming in.
    // Can be expressed as horizontal fov or vertical fov.
    float fov  = 90.0f;
    // Length from the aperture/eye to the canvas/image plane. Higher values is zooming in, while lower zoom out.
    float focal_length = 20.0f;
    // Size of the canvas/image plane/sensor/screen. Larger values means more thing on screen (zooming out), while
    // lower is zooming in. Can be expressed as horizontal size or vertical size.
    float canvas_size = 0;

    // The physical dimension of the film that would be used in a real camera. The angle of view depends on this value.
    float film_aperture_width  = 21.0f;
    float film_aperture_height = 21.0f;


    AABB image_plane {
            static_cast<int>(-((film_aperture_width  / 2) / focal_length) * distance_to_canvas),
            static_cast<int>( ((film_aperture_height / 2) / focal_length) * distance_to_canvas),
            static_cast<int>( ((film_aperture_width  / 2) / focal_length) * distance_to_canvas),
            static_cast<int>(-((film_aperture_height / 2) / focal_length) * distance_to_canvas),
    };


    float distance_to_canvas = 1.0f;  // Setting it the same as near for now.

    glm::mat3 cached_rotation_matrix    = glm::mat3();  // Initialized as identity matrix.
    glm::mat4 cached_perspective_matrix = glm::mat4();  // Initialized as identity matrix.
};


vec2 Normalize(const Camera& camera, const AABB& viewport, float x, float y)
{
    const int image_width  = viewport.right  - viewport.left;
    const int image_height = viewport.bottom - viewport.top;

    const float r = camera.image_plane.right;
    const float l = camera.image_plane.left;
    const float t = camera.image_plane.top;
    const float b = camera.image_plane.bottom;
    const float normalized_device_coordinate_x = (2 * x) / (r - l) - (r + l) / (r - l);
    const float normalized_device_coordinate_y = (2 * y) / (t - b) - (t + b) / (t - b);

    const float raster_x = ((normalized_device_coordinate_x + 1) / 2) * image_width;
    const float raster_y = ((normalized_device_coordinate_y + 1) / 2) * image_height;

    return vec2(raster_x, raster_y);
}



















