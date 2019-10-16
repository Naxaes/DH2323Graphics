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


int main()
{
	constexpr int width  = 400;
	constexpr int height = 400;

    InitializeSDL2();

	Window window = CreateWindow("Skeleton", width, height);
    Clear(window, WHITE);

    bool mouse_button_down = false;
	bool running = true;
	while (running)
	{
		// --- HANDLE EVENTS ----
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
		    if (event.type == SDL_QUIT)
            {
		        running = false;
            }
		    else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
		        if (event.button.button == SDL_BUTTON_LEFT)
                    mouse_button_down = true;
            }
		    else if (event.type == SDL_MOUSEBUTTONUP)
		    {
		        if (event.button.button == SDL_BUTTON_LEFT)
                    mouse_button_down = false;
		    }
		    else if (event.type == SDL_MOUSEMOTION)
		    {
		        if (mouse_button_down)
                    DrawPixel(window, glm::uvec2(event.button.x, event.button.y), BLACK);
		    }
			else if (event.type == SDL_KEYDOWN)
            {
				if (event.key.keysym.sym == SDLK_BACKSPACE)
                    Clear(window, WHITE);
				else if (event.key.keysym.sym == SDLK_p)
					ScreenShot(window, "../screenshot.png");
            }
		}


		// --- RENDER ----
        Render(window);
	}


	// ---- CLEAN UP ----
    DestroyWindow(window);
	SDL_Quit();
}