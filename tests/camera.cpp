#include "test.h"
#include "camera.h"

Test(Normalization)
{
    AABB viewport {0, 0, 100, 100};
    Camera camera;

    vec2 point = Normalize(camera, viewport, 0, 0);

    Check(point.x, ==, 0);
    Check(point.y, ==, 0);
}


int main()
{
    RunAllTests();
}