#pragma once

#include <glm/glm.hpp>

#include "debug.h"

using glm::ivec2;
using glm::vec2;


struct Rectangle
{
    int left;
    int top;
    int right;
    int bottom;
};


enum class IntersectionType
{
    INTERSECTION, COLLINEAR, PARALLEL_NON_INTERSECTION, NON_PARALLEL_NON_INTERSECTION
};

template <typename T>
struct Intersection
{
    const T point;
    const IntersectionType type;
};


Intersection<ivec2> LineLineIntersection(const ivec2& start1, const ivec2& stop1, const ivec2& start2, const ivec2& stop2)
{
    using glm::round;

    const ivec2 p = start1;
    const ivec2 r = stop1 - start1;
    const ivec2 q = start2;
    const ivec2 s = stop2 - start2;

    const auto Cross = [](const ivec2& a, const ivec2& b) -> int { return a.x * b.y - a.y * b.x; };

    // https://stackoverflow.com/a/565282/6486738
    // Intersection:  p + t * r = q + u * s
    // Cross product: v X w = v.x * w.y − v.y * w.x

    // Solving for t:
    // (p + t r) × s = (q + u s) × s
    // t (r × s) = (q − p) × s
    // t = (q − p) × s / (r × s)

    // Solving for u
    // (p + t r) × r = (q + u s) × r
    // u (s × r) = (p − q) × r
    // u = (p − q) × r / (s × r)
    // u = (q − p) × r / (r × s)


    // If r × s = 0 and (q − p) × r = 0, then the two lines are collinear.
    // If r × s = 0 and (q − p) × r ≠ 0, then the two lines are parallel and non-intersecting.
    // If r × s ≠ 0 and 0 ≤ t ≤ 1 and 0 ≤ u ≤ 1, the two line segments meet at the point p + t r = q + u s.
    // Otherwise, the two line segments are not parallel but do not intersect.

    int dividend = Cross(q - p, s);
    int divisor  = Cross(r, s);


    if (divisor == 0 and dividend == 0)
        return { ivec2(), IntersectionType::COLLINEAR };                          // Collinear.
    if (divisor == 0 and dividend != 0)
        return { ivec2(), IntersectionType::PARALLEL_NON_INTERSECTION };         // Parallel non-intersecting.

    float t = Cross(q - p, s) / static_cast<float>(Cross(r, s));
    float u = Cross(q - p, r) / static_cast<float>(Cross(r, s));

    if (divisor != 0 and 0 <= t and t <= 1 and 0 <= u and u <= 1)
        return { p + ivec2(round(t * vec2(r))), IntersectionType::INTERSECTION};  // Intersecting at p + t * r = q + u * s
    else
        return { ivec2(), IntersectionType::NON_PARALLEL_NON_INTERSECTION };     // Non-parallel non-intersecting.
};


template <typename T>
struct LR_Intersection
{
    enum Type { NO_INTERSECTION, ONE_INTERSECTION, TWO_INTERSECTIONS };

    T    first;
    T    second;
    Type type;
};

LR_Intersection<ivec2> LineRectIntersection(const Rectangle& rectangle, const glm::ivec2& start, const glm::ivec2& stop)
{
    using glm::distance;

    const ivec2 bottom_right = ivec2(rectangle.right-1, rectangle.bottom-1);
    const ivec2 top_left     = ivec2(rectangle.left,    rectangle.top);
    const ivec2 bottom_left  = ivec2(rectangle.left,    rectangle.bottom-1);
    const ivec2 top_right    = ivec2(rectangle.right-1, rectangle.top);

    const Intersection<ivec2> left_intersection   = LineLineIntersection(start, stop, top_left,     bottom_left);
    const Intersection<ivec2> top_intersection    = LineLineIntersection(start, stop, top_left,     top_right);
    const Intersection<ivec2> right_intersection  = LineLineIntersection(start, stop, bottom_right, top_right);
    const Intersection<ivec2> bottom_intersection = LineLineIntersection(start, stop, bottom_right, bottom_left);

    ivec2 intersections[2];
    int intersection_count = 0;

    if (left_intersection.type   == IntersectionType::INTERSECTION)  intersections[intersection_count++] = left_intersection.point;
    if (top_intersection.type    == IntersectionType::INTERSECTION)  intersections[intersection_count++] = top_intersection.point;
    if (right_intersection.type  == IntersectionType::INTERSECTION)  intersections[intersection_count++] = right_intersection.point;
    if (bottom_intersection.type == IntersectionType::INTERSECTION)  intersections[intersection_count++] = bottom_intersection.point;

    if (intersection_count == 0)
        return { ivec2(0), ivec2(0), LR_Intersection<ivec2>::NO_INTERSECTION };
    else if (intersection_count == 1)
        return { intersections[0], ivec2(0), LR_Intersection<ivec2>::ONE_INTERSECTION };
    else
        return { intersections[0], intersections[1], LR_Intersection<ivec2>::TWO_INTERSECTIONS };

}

struct AABB
{
    int left, top, right, bottom;
};

AABB BoundingBox(const ivec2& v0, const ivec2& v1, const ivec2& v2)
{
    using std::min;
    using std::max;

    int left   = min({v0.x, v1.x, v2.x});
    int top    = min({v0.y, v1.y, v2.y});
    int right  = max({v0.x, v1.x, v2.x});
    int bottom = max({v0.y, v1.y, v2.y});

    return { left, top, right, bottom };
}

bool Contains(const AABB& area, const ivec2& point)
{
    return area.left >= point.x and point.x < area.right and area.top >= point.y and point.y < area.bottom;
}

bool IsCompletelyOutsideOf(const AABB& area, const AABB& candidate)
{
    return candidate.right  < area.left or candidate.left >= area.right or
           candidate.bottom < area.top  or candidate.top  >= area.bottom;
}

bool IsCompletelyInsideOf(const AABB& area, const AABB& candidate)
{
    return candidate.left >= area.left and candidate.right < area.right and
           candidate.top  >= area.top and  candidate.top  < area.bottom;
}


bool CohenSutherlandLineClip(const AABB& viewport, ivec2& v0, ivec2& v1)
{
    // https://en.wikipedia.org/wiki/Cohen–Sutherland_algorithm

    constexpr unsigned char INSIDE = 0b0000;
    constexpr unsigned char LEFT   = 0b0001;
    constexpr unsigned char RIGHT  = 0b0010;
    constexpr unsigned char BOTTOM = 0b0100;
    constexpr unsigned char TOP    = 0b1000;

    const auto OutCode = [&](const ivec2& v) -> unsigned char
    {
        unsigned char code = INSIDE;         // initialised as being inside of [[clip window]]

        if (v.x < viewport.left)             // to the left of clip window
            code |= LEFT;
        else if (v.x >= viewport.right)      // to the right of clip window
            code |= RIGHT;
        if (v.y < viewport.top)              // below the clip window
            code |= TOP;
        else if (v.y >= viewport.bottom)     // above the clip window
            code |= BOTTOM;

        return code;
    };


    const auto IntersectionPoint = [&](const unsigned char code, const ivec2& v0, const ivec2& v1) -> ivec2
    {
        // Now find the intersection point;
        // use formulas:
        //   slope = (y1 - y0) / (x1 - x0)
        //   x = x0 + (1 / slope) * (ym - y0), where ym is viewport.top or viewport.bottom
        //   y = y0 + slope * (xm - x0), where xm is viewport.left or viewport.right
        // No need to worry about divide-by-zero because, in each case, the
        // outcode bit being tested guarantees the denominator is non-zero

        if (code & TOP)
        {
            // point is above the clip window
            const int x = static_cast<int>(round(v0.x + (v1.x - v0.x) * (viewport.top - v0.y) / static_cast<float>(v1.y - v0.y)));
            const int y = viewport.top;
            return ivec2(x, y);
        }
        else if (code & BOTTOM)
        {
            // point is below the clip window
            const int x = static_cast<int>(round(v0.x + (v1.x - v0.x) * (viewport.bottom - v0.y) / static_cast<float>(v1.y - v0.y)));
            const int y = viewport.bottom - 1;
            return ivec2(x, y);
        }
        else if (code & RIGHT)
        {
            // point is to the right of clip window
            const int y = static_cast<int>(round(v0.y + (v1.y - v0.y) * (viewport.right - v0.x) / static_cast<float>(v1.x - v0.x)));
            const int x = viewport.right - 1;
            return ivec2(x, y);
        }
        else if (code & LEFT)
        {
            // point is to the left of clip window
            const int y = static_cast<int>(round(v0.y + (v1.y - v0.y) * (viewport.left - v0.x) / static_cast<float>(v1.x - v0.x)));
            const int x = viewport.left;
            return ivec2(x, y);
        }
        else
        {
            Assert(false, "What!?");
            return {};
        }
    };


    unsigned char code_v0 = OutCode(v0);
    unsigned char code_v1 = OutCode(v1);

    while (true)
    {
        if ((code_v0 | code_v1) == 0)      // Both are in the viewport region. Accept!
            return true;
        else if ((code_v0 & code_v1) != 0) // Both are in the same region (but not the viewport), or the same side! Reject!
            return false;

        // Else one is in the viewport, or they are both outside but could cross the viewport.

        // With both tests failed we calculate the line segment to clip from an outside point to an intersection
        // with clip edge.

        // At least one endpoint is outside the clip rectangle; pick it, or else pick arbitrarily.
        if (code_v0 != INSIDE)
        {
            // Move the outside point to intersection point.
            v0 = IntersectionPoint(code_v0, v0, v1);
            code_v0 = OutCode(v0);
        }
        else
        {
            // Move the outside point to intersection point.
            v1 = IntersectionPoint(code_v1, v0, v1);
            code_v1 = OutCode(v1);
        }
    }
}
