#include "test.h"
#include "SDLhelper.h"


Test(Initialize)
{
    Check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER), >=, 0);
}


Test(CreateWindow)
{
    Window window = CreateWindow("test", 10, 10);

    Check(window.handle,   !=, nullptr);
    Check(window.renderer, !=, nullptr);
    Check(window.screen,   !=, nullptr);
    Check(window.pixels,   !=, nullptr);

    DestroyWindow(window);
}

Test(DrawPixel)
{
    Window window = CreateWindow("test", 10, 10);

    for (int row = 0; row < 10; ++row)
        for (int column = 0; column < 10; ++column)
            DrawPixel(window, glm::uvec2(column, row));


    for (int row = 0; row < 10; ++row)
        for (int column = 0; column < 10; ++column)
            Check(window.pixels[row * 10 + column], ==, ColorCode(WHITE));

    DestroyWindow(window);
}

Test(Clear)
{
    options.color->check = Colors::BOLD_BLUE;

    Window window = CreateWindow("test", 10, 10);

    for (int row = 0; row < 10; ++row)
        for (int column = 0; column < 10; ++column)
            DrawPixel(window, glm::uvec2(column, row));


    Clear(window);

    for (int row = 0; row < 10; ++row)
        for (int column = 0; column < 10; ++column)
        Check(window.pixels[row * 10 + column], ==, ColorCode(BLACK));

    DestroyWindow(window);
}

enum XXX { Something };

struct XXY
{
    int a = 5;
    float y = 3.0f;
    char x = 2;

    bool operator== (const XXY& other) const { return true; }
};

Test(StringRepresentation)
{
    options.flags = Options::OUTPUT_EVERYTHING;

    Check(Something, ==, 4);
    Check(XXY(), ==, XXY());

    Check(0.04 - 0.00001, ==, 0.04);
    Check(0.04f - 0.0001, ==, 0.04f);
}


int main()
{
    Options options;

    RunAllTests(options);
}