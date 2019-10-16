#pragma once

#include <vector>
#include <cmath>

#include <glm/glm.hpp>





struct Pixel
{
    glm::ivec2 location = glm::ivec2(0);

    float z = 0;

    glm::vec3 inverted_world_position = glm::vec3();

    Pixel() = default;
    Pixel(int x, int y, float z, const glm::vec3& inverted_world_position) :
            location(x, y), z(z), inverted_world_position(inverted_world_position) {}

    Pixel& operator= (const Pixel& other) = default;

};


template <typename T>
[[gnu::const]] [[gnu::hot]]
std::vector<T> Interpolate(const T start, const T stop, const unsigned samples)
{
    if (samples == 1)
        return { start };

    const float delta = (stop - start) / static_cast<float>(samples-1);  // Avoiding off-by-one error.
    float value = start;

    std::vector<T> result;
    result.reserve(samples);

    for (unsigned i = 0; i < samples; ++i)
    {
        if (std::is_same<T, float>::value or std::is_same<T, double>::value)
            result.push_back(value);
        else
            result.push_back(std::round(value));

        value += delta;
    }

    return result;
}


template <>
[[gnu::const]] [[gnu::hot]]
std::vector<glm::vec3> Interpolate(const glm::vec3 start, const glm::vec3 stop, const unsigned samples)
{
    if (samples == 1)
        return { start };

    const glm::vec3 delta = (stop - start) / static_cast<float>(samples-1);  // Avoiding off-by-one error.
    glm::vec3 value = start;

    std::vector<glm::vec3> result;
    result.reserve(samples);

    for (unsigned i = 0; i < samples; ++i)
    {
        result.push_back(value);
        value += delta;
    }

    return result;
}


// [[gnu::const]]
// Pixel Interpolate(const glm::ivec2& p, const Pixel& v0, const Pixel& v1, const Pixel& v2)
// {
//     const float w0 = ((v1.y - v2.y) * (p.x  - v2.x) + (v2.x - v1.x) * (p.y  - v2.y)) / static_cast<float>
//                      ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
//
//     const float w1 = ((v2.y - v0.y) * (p.x  - v2.x) + (v0.x - v2.x) * (p.y  - v2.y)) / static_cast<float>
//                      ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
//
//     const float w2 = 1 - w0 - w1;
//
//     // Assert(w0 + w1 + w2 > 0.9999f and w0 + w1 + w2 < 1.0001f, "WRONG!! %f + %f + %f = %f", w0, w1, w2, w0 + w1 + w2);
//
//     const int x = static_cast<int>(v0.x * w0 + v1.x * w1 + v2.x * w2);
//     const int y = static_cast<int>(v0.y * w0 + v1.y * w1 + v2.y * w2);
//     const float z = v0.inverse_depth * w0 + v1.inverse_depth * w1 + v2.inverse_depth * w2;
//     const glm::vec3 w = v0.inverted_world_position * w0 + v1.inverted_world_position * w1 + v2.inverted_world_position * w2;
//
//     return { x, y, z, w };
// }