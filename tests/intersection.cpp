#include "test.h"

#include "intersection.h"


Test(CollisionError)
{
    const ivec2 start1 (266, 266);
    const ivec2 stop1  (400, 0);
    const ivec2 start2 (399, 399);
    const ivec2 stop2  (399, 0);

    const Intersection<ivec2> result = LineLineIntersection(start1, stop1, start2, stop2);

    Check(result.type, ==, IntersectionType::INTERSECTION);

    Check(result.point.x, ==, 0);
    Check(result.point.y, ==, 399);
}


Test(Collision)
{
    const ivec2 start1 (0, 0);
    const ivec2 stop1  (100, 100);
    const ivec2 start2 (100, 0);
    const ivec2 stop2  (0, 100);

    const Intersection<ivec2> result = LineLineIntersection(start1, stop1, start2, stop2);

    Check(result.type, ==, IntersectionType::INTERSECTION);

    Check(result.point.x, ==, 50);
    Check(result.point.y, ==, 50);
}

Test(CollisionEdge)
{
    const ivec2 start1 (133, 266);
    const ivec2 stop1  (400, 400);
    const ivec2 start2 (400, 400);
    const ivec2 stop2  (0,   400);

    const Intersection<ivec2> result = LineLineIntersection(start1, stop1, start2, stop2);

    Check(result.type, ==, IntersectionType::INTERSECTION);

    Check(result.point.x, ==, 400);
    Check(result.point.y, ==, 400);
}

Test(CollisionEdge2)
{
    const ivec2 start1 (400, 400);
    const ivec2 stop1  (0,   400);

    const ivec2 start2 (399, 399);
    const ivec2 stop2  (399,   0);
    const ivec2 stop3  (0,   399);

    const Intersection<ivec2> result1 = LineLineIntersection(start1, stop1, start2, stop2);
    const Intersection<ivec2> result2 = LineLineIntersection(start1, stop1, start2, stop3);

    Check(result1.type, ==, IntersectionType::NON_PARALLEL_NON_INTERSECTION);
    Check(result2.type, ==, IntersectionType::PARALLEL_NON_INTERSECTION);
}

Test(Collinear)
{
    const ivec2 start1 (0, 0);
    const ivec2 stop1  (100, 100);
    const ivec2 start2 (0, 0);
    const ivec2 stop2  (100, 100);

    const Intersection<ivec2> result = LineLineIntersection(start1, stop1, start2, stop2);

    Check(result.type, ==, IntersectionType::COLLINEAR);
}


Test(Parallel)
{
    const ivec2 start1 (0, 0);
    const ivec2 stop1  (0, 100);
    const ivec2 start2 (10, 0);
    const ivec2 stop2  (10, 100);

    const Intersection<ivec2> result = LineLineIntersection(start1, stop1, start2, stop2);

    Check(result.type, ==, IntersectionType::PARALLEL_NON_INTERSECTION);
}

Test(NonIntersecting)
{
    const ivec2 start1 (0, 0);
    const ivec2 stop1  (50, 50);
    const ivec2 start2 (0,  100);
    const ivec2 stop2  (100, 100);

    const Intersection<ivec2> result = LineLineIntersection(start1, stop1, start2, stop2);

    Check(result.type, ==, IntersectionType::NON_PARALLEL_NON_INTERSECTION);
}

Test(LineRectIntersection)
{

    Rectangle rectangle { 0, 0, 400, 400 };

    // Diagonal
    ivec2 intersection1 = LineRectIntersection(rectangle, ivec2(50, 50), ivec2(405, 405));

    // Diagonal (other way)
    ivec2 intersection2 = LineRectIntersection(rectangle, ivec2(405, 405), ivec2(50, 50));

    // Vertical
    ivec2 intersection3 = LineRectIntersection(rectangle, ivec2(50, 50),  ivec2(50, 500));

    // Horizontal
    ivec2 intersection4 = LineRectIntersection(rectangle, ivec2(50, 50),  ivec2(500, 50));

    // Angled
    ivec2 intersection5 = LineRectIntersection(rectangle, ivec2(159, 23), ivec2(-35, 504));

    Check(intersection1.x, ==, 399);
    Check(intersection1.y, ==, 399);

    Check(intersection2.x, ==, 399);
    Check(intersection2.y, ==, 399);

    Check(intersection3.x, ==, 50);
    Check(intersection3.y, ==, 399);

    Check(intersection4.x, ==, 399);
    Check(intersection4.y, ==, 50);

    Check(intersection5.x, ==, 7);
    Check(intersection5.y, ==, 399);
}


Test(NoLineRectIntersection)
{

    Rectangle rectangle { 0, 0, 400, 400 };

    ivec2 intersection1 = LineRectIntersection(rectangle, ivec2(50, 50),   ivec2(2, 2));
    ivec2 intersection2 = LineRectIntersection(rectangle, ivec2(33, 150),  ivec2(235, 50));

    Check(intersection1.x, ==, 2);
    Check(intersection1.y, ==, 2);

    Check(intersection2.x, ==, 235);
    Check(intersection2.y, ==, 50);
}

int main()
{
    RunAllTests();
}


