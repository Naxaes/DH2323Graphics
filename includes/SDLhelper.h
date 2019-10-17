#include <iostream>
#include <ctime>
#include <iomanip>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include "debug.h"

const glm::vec4 BLACK ( 0.0f, 0.0f, 0.0f, 0.0f );
const glm::vec4 GREY  ( 0.5f, 0.5f, 0.5f, 0.5f );
const glm::vec4 WHITE ( 1.0f, 1.0f, 1.0f, 1.0f );

const glm::vec4 RED   ( 1.0f, 0.0f, 0.0f, 1.0f );
const glm::vec4 GREEN ( 0.0f, 1.0f, 0.0f, 1.0f );
const glm::vec4 BLUE  ( 0.0f, 0.0f, 1.0f, 1.0f );

const glm::vec4 CYAN    ( 0.0f, 1.0f, 1.0f, 1.0f );
const glm::vec4 YELLOW  ( 1.0f, 1.0f, 0.0f, 1.0f );
const glm::vec4 MAGENTA ( 1.0f, 0.0f, 1.0f, 1.0f );


struct Clock
{
	unsigned current_tick;
	unsigned last_tick;

	Clock() : current_tick(SDL_GetTicks()), last_tick(current_tick) {}
};

float Tick(Clock& clock)
{
	clock.last_tick = clock.current_tick;
	clock.current_tick = SDL_GetTicks();

	unsigned delta = clock.current_tick - clock.last_tick;
	return delta / 1000.0f;
}

struct Window
{
	SDL_Window*   handle   = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture*  screen   = nullptr;
	Uint32*       pixels   = nullptr;

	unsigned width  = 0;
	unsigned height = 0;
};

Uint8 MapFloatToUint8(const float color)
{
    return static_cast<Uint8>(color * 255);
}

Uint32 ColorCode(const glm::vec4& color)
{
    return  (MapFloatToUint8(color.a) << 24) +
            (MapFloatToUint8(color.r) << 16) +
            (MapFloatToUint8(color.g) << 8 ) +
            (MapFloatToUint8(color.b));
}

Uint32 ColorCode(const glm::vec3& color)
{
	return  (MapFloatToUint8(1.0f)    << 24) +
			(MapFloatToUint8(color.r) << 16) +
			(MapFloatToUint8(color.g) << 8 ) +
			(MapFloatToUint8(color.b));
}

void InitializeSDL2()
{
	Assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0, "Couldn't initialize SDL. %s", SDL_GetError());
}


Window CreateWindow(const std::string& name, const int width, const int height)
{
	SDL_Window*   window;
	SDL_Renderer* renderer;

	unsigned window_flags   = 0;                       // https://wiki.libsdl.org/SDL_WindowFlags
	unsigned renderer_flags = SDL_RENDERER_SOFTWARE;   // https://wiki.libsdl.org/SDL_RendererFlags

	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

	renderer = SDL_CreateRenderer(window, -1, renderer_flags);

	// https://wiki.libsdl.org/SDL_CreateTexture
	SDL_Texture* screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

	Assert(window,   "Couldn't create window. %s",   SDL_GetError());
	Assert(renderer, "Couldn't create renderer. %s", SDL_GetError());
	Assert(screen,   "Couldn't create texture. %s",  SDL_GetError());

	Uint32* pixels = new Uint32[width * height];

	return { window, renderer, screen, pixels, static_cast<unsigned>(width), static_cast<unsigned>(height) };
}

void DestroyWindow(Window& window)
{
	delete[] window.pixels;
	SDL_DestroyTexture(window.screen);
	SDL_DestroyRenderer(window.renderer);
	SDL_DestroyWindow(window.handle);
}

void FillWindow(Window& window, const glm::vec3* colors)
{
	for (unsigned row = 0; row < window.height; ++row)
		for (unsigned column = 0; column < window.width; ++column)
			window.pixels[row * window.width + column] = ColorCode(colors[row * window.width + column]);
}

void FillWindow(Window& window, const Uint32* colors)
{
    for (unsigned row = 0; row < window.height; ++row)
        for (unsigned column = 0; column < window.width; ++column)
            window.pixels[row * window.width + column] = colors[row * window.width + column];
}

void DrawPixel(Window& window, const glm::uvec2& location, const glm::vec4& color = WHITE)
{
	Assert(location.x >= 0 and location.x < window.width and location.y >= 0 or location.y < window.height, "%u, %u", location.x, location.y);

	// 8 bits Alpha, 8 bits Red, 8 bits Green, 8 bits Blue
	const Uint32 color_code = ColorCode(color);
	window.pixels[location.y * window.width + location.x] = color_code;
}

void DrawPixel(Window& window, const glm::uvec2& location, const glm::vec3& color)
{
	DrawPixel(window, location, glm::vec4(color, 1.0f));
}

void Clear(Window& window, const glm::vec4& color = BLACK)
{
	// Clear pixel buffer.
	std::memset(window.pixels, ColorCode(color), window.width * window.height * sizeof(Uint32));

	// // Clear screen.
	SDL_UpdateTexture(window.screen, nullptr, window.pixels, window.width * sizeof(Uint32));

    Uint8 r = MapFloatToUint8(color.r);
    Uint8 g = MapFloatToUint8(color.g);
    Uint8 b = MapFloatToUint8(color.b);
    Uint8 a = MapFloatToUint8(color.a);

	Assert(SDL_SetRenderDrawColor(window.renderer, r, g, b, a) == 0, "Error: %s", SDL_GetError());
	Assert(SDL_RenderClear(window.renderer) == 0, "Error: %s", SDL_GetError());
}


void Render(Window& window)
{
	Assert(SDL_UpdateTexture(window.screen, nullptr, &window.pixels[0], window.width * 4) == 0, "Error: %s", SDL_GetError());

	Assert(SDL_RenderCopy(window.renderer, window.screen, nullptr, nullptr) == 0, "Error: %s", SDL_GetError());
	SDL_RenderPresent(window.renderer);
}


void ScreenShot(const Window& window, const std::string& filename)
{
	int width, height;
	SDL_GetWindowSize(window.handle, &width, &height);

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(window.pixels, width, height, 32, width * 4, 0, 0, 0, 0);
	SDL_SaveBMP(surface, filename.c_str());
	SDL_FreeSurface(surface);

	std::cout << "Screenshot saved! " << filename << std::endl;
}

void ScreenShot(const Window& window)
{
	using namespace std;

	time_t raw_time;
	tm*  time_info;
	char buffer[80];

	time(&raw_time);
	time_info = localtime(&raw_time);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", time_info);
	string date_and_time(buffer);

	ScreenShot(window, date_and_time + ".bmp");
}
