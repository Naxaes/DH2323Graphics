#include <vector>
#include <random>

#include "SDLhelper.h"


enum class State { RAINBOW, STAR_FIELD };

// Using integers as the compiler cries and can't handle floats.
template <int start, int stop>
float random_float()
{
    static std::random_device rd;
    static std::mt19937 generator(rd());
    static std::uniform_real_distribution<float> distribution(start, stop);

    return distribution(generator);
}


template <typename T>
std::vector<T> Interpolate(const T start, const T stop, const unsigned samples = 1)
{
    if (samples == 1)
        return { start };

    const T delta = (stop - start) / static_cast<float>(samples-1);  // Avoiding off-by-one error.
    T value = start;

    std::vector<T> result;
    result.reserve(samples);

    for (unsigned i = 0; i < samples; ++i)
    {
        result.push_back(value);
        value += delta;
    }

    return result;
}


void Rainbow(Window& window)
{
    int width, height;
    SDL_GetWindowSize(window.handle, &width, &height);

    const glm::vec4 top_left     = RED;
    const glm::vec4 top_right    = BLUE;
    const glm::vec4 bottom_left  = YELLOW;
    const glm::vec4 bottom_right = GREEN;

    const std::vector<glm::vec4> left_column  = Interpolate(top_left,  bottom_left,  height);
    const std::vector<glm::vec4> right_column = Interpolate(top_right, bottom_right, height);

    for (unsigned row = 0; row < height; ++row)
    {
        const std::vector<glm::vec4> row_colors = Interpolate(left_column[row], right_column[row], width);

        for (unsigned column = 0; column < width; ++column)
        {
            DrawPixel(window, glm::uvec2(column, row), row_colors[column]);
        }
    }

}


std::vector<glm::vec3> CreateStarField(const unsigned star_count)
{
    std::vector<glm::vec3> stars_in_world_position;
    stars_in_world_position.reserve(star_count);

    for (unsigned i = 0; i < star_count; ++i)
        stars_in_world_position.emplace_back(random_float<-1, 1>(), random_float<-1, 1>(), random_float<0, 1>());

    return stars_in_world_position;
}

void MoveStarField(std::vector<glm::vec3>& stars_in_world_position, const glm::vec3& velocity)
{
    for (auto& position : stars_in_world_position)
    {
        position -= velocity;
        if (position.z <= 0)
            position.z = 1;
    }
}

void DrawStarField(Window& window, const std::vector<glm::vec3>& stars_in_world_position)
{
    int width, height;
    SDL_GetWindowSize(window.handle, &width, &height);

    const float focal_length = width / 2.0f;

    for (const auto& world_position : stars_in_world_position)
    {
        if (world_position.z <= 0)
            continue;

        const float u = focal_length * (world_position.x / world_position.z) + width  / 2.0f;
        const float v = focal_length * (world_position.y / world_position.z) + height / 2.0f;

        const glm::vec4 color = (0.2f * WHITE) / (world_position.z * world_position.z);

        const auto min = glm::vec4(0, 0, 0, 1);
        const auto max = glm::vec4(1, 1, 1, 1);

        DrawPixel(window, glm::uvec2(u, v), glm::clamp(color, min, max));
    }
}


int main()
{
	constexpr int width  = 400;
	constexpr int height = 400;


    InitializeSDL2();

	Window window = CreateWindow("Lab1", width, height);

    Clock clock;

    std::vector<glm::vec3> star_field = CreateStarField(1000);
    glm::vec3 star_velocity (0.0f, 0.0f, 0.1f);
    const float slow_star_speed   = 0.05f;
    const float normal_star_speed = 0.2f;
    const float fast_star_speed   = 0.5f;

	bool running = true;

	State state = State::RAINBOW;

	while (running)
	{
	    const float delta = Tick(clock);

		// --- HANDLE EVENTS ----
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
            if (event.type == SDL_QUIT)
            {
		        running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE)
                    Clear(window, WHITE);
                else if (event.key.keysym.sym == SDLK_p)
                    ScreenShot(window, "../screenshot.png");
                else if (event.key.keysym.sym == SDLK_w)
                    star_velocity.z = fast_star_speed;
                else if (event.key.keysym.sym == SDLK_a)
                    star_velocity.x = -fast_star_speed;
                else if (event.key.keysym.sym == SDLK_d)
                    star_velocity.x = fast_star_speed;
                else if (event.key.keysym.sym == SDLK_s)
                    star_velocity.z = slow_star_speed;
                else if (event.key.keysym.sym == SDLK_SPACE)
                {
                    if (state == State::RAINBOW)
                        state = State::STAR_FIELD;
                    else if (state == State::STAR_FIELD)
                        state = State::RAINBOW;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_w)
                    star_velocity.z = normal_star_speed;
                else if (event.key.keysym.sym == SDLK_a)
                    star_velocity.x = 0;
                else if (event.key.keysym.sym == SDLK_d)
                    star_velocity.x = 0;
                else if (event.key.keysym.sym == SDLK_s)
                    star_velocity.z = normal_star_speed;
            }
		}


		if (state == State::RAINBOW)
        {
            // --- RENDER ----
            Rainbow(window);
        }
        else
        {
            // --- UPDATE ----
            MoveStarField(star_field, star_velocity * delta);

            // --- RENDER ----
            Clear(window, BLACK);
            DrawStarField(window, star_field);
        }

        Render(window);
	}


	// ---- CLEAN UP ----
    DestroyWindow(window);
	SDL_Quit();
}
