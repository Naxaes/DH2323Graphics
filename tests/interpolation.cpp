#include "test.h"
#include "interpolation.h"

Test(InterpolationForwards)
{
    const int x = 0;
    const int y = 200;
    const unsigned samples = 201;

    const std::vector<int> result = Interpolate(x, y, samples);

    // END POINTS
    Check(result[ 0 ], ==,  0 );
    Check(result[200], ==, 200);

    // MIDDLE POINTS
    Check(result[99],  ==, 99);
    Check(result[100], ==, 100);
    Check(result[101], ==, 101);

    // QUARTER POINTS
    Check(result[50],  ==,  50);
    Check(result[150], ==, 150);
}

Test(InterpolationBackwards)
{
    const int x = 200;
    const int y = 0;
    const unsigned samples = 201;

    const std::vector<int> result = Interpolate(x, y, samples);

    // END POINTS
    Check(result[ 0 ], ==, 200);
    Check(result[200], ==,  0 );

    // MIDDLE POINTS
    Check(result[99],  ==, 101);
    Check(result[100], ==, 100);
    Check(result[101], ==,  99);

    // QUARTER POINTS
    Check(result[50],  ==, 150);
    Check(result[150], ==,  50);
}

Test(DiscreteInterpolationForwards)
{
    options.flags = Options::OUTPUT_NOTHING;

    const int x = 0;
    const int y = 200;
    const unsigned samples = 201;

    const std::vector<int> result = Interpolate(x, y, samples);

    for (int i = 0; i < static_cast<int>(samples); ++i)
        Check(result[i], ==, i);
}

Test(DiscreteInterpolationForwardMulitpleSamples)
{
    options.flags = Options::OUTPUT_NOTHING;

    const int x = 0;
    const int y = 200;
    const unsigned samples = 400;

    const std::vector<int> result = Interpolate(x, y, samples);

    for (int i = 0, j = 0; i < static_cast<int>(result.size()); i += 2, j += 1)
        Check(result[i], ==, j);
}


Test(BarycentricInterpolation)
{
    const Pixel a { 0,   0,  0, glm::vec3() };
    const Pixel b { 75, 75,  3, glm::vec3() };
    const Pixel c { 150, 0,  6, glm::vec3() };

    const Pixel result1 = Interpolate(a, a, b, c);
    const Pixel result2 = Interpolate(b, a, b, c);
    const Pixel result3 = Interpolate(c, a, b, c);
    const Pixel result4 = Interpolate({ 75, 25, 0, glm::vec3() }, a, b, c);

    Check(result1.x, ==, a.x);
    Check(result1.y, ==, a.y);
    Check(result1.inverse_depth, ==, a.inverse_depth);

    Check(result2.x, ==, b.x);
    Check(result2.y, ==, b.y);
    Check(result2.inverse_depth, ==, b.inverse_depth);

    Check(result3.x, ==, c.x);
    Check(result3.y, ==, c.y);
    Check(result3.inverse_depth, ==, c.inverse_depth);

    CheckReal(result4.x, a.x * (1/3.0) + b.x * (1/3.0) + c.x * (1/3.0), 0.000001);
    CheckReal(result4.y, a.y * (1/3.0) + b.y * (1/3.0) + c.y * (1/3.0), 0.000001);
    CheckReal(result4.inverse_depth, a.inverse_depth * (1/3.0) + b.inverse_depth * (1/3.0) + c.inverse_depth * (1/3.0), 0.0001);
}

int main()
{
    RunAllTests();
}