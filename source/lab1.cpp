//// Using integers as the compiler cries and can't handle floats.
//template <int start, int stop>
//float random_float()
//{
//    static std::random_device rd;
//    static std::mt19937 generator(rd());
//    static std::uniform_real_distribution<float> distribution(start, stop);
//
//    return distribution(generator);
//}
//
//
//template <typename T>
//std::vector<T> Interpolate(const T start, const T stop, const unsigned samples = 1)
//{
//    if (samples == 1)
//        return { start };
//
//    const T delta = (stop - start) / static_cast<float>(samples-1);  // Avoiding off-by-one error.
//    T value = start;
//
//    std::vector<T> result;
//    result.reserve(samples);
//
//    for (unsigned i = 0; i < samples; ++i)
//    {
//        result.push_back(value);
//        value += delta;
//    }
//
//    return result;
//}
//
//
//void Rainbow(Window& window)
//{
//    int width, height;
//    SDL_GetWindowSize(window.handle, &width, &height);
//
//    const glm::vec4 top_left     = RED;
//    const glm::vec4 top_right    = BLUE;
//    const glm::vec4 bottom_left  = YELLOW;
//    const glm::vec4 bottom_right = GREEN;
//
//    const std::vector<glm::vec4> left_column  = Interpolate(top_left,  bottom_left,  height);
//    const std::vector<glm::vec4> right_column = Interpolate(top_right, bottom_right, height);
//
//    for (unsigned row = 0; row < height; ++row)
//    {
//        const std::vector<glm::vec4> row_colors = Interpolate(left_column[row], right_column[row], width);
//
//        for (unsigned column = 0; column < width; ++column)
//        {
//            DrawPixel(window, glm::uvec2(column, row), row_colors[column]);
//        }
//    }
//
//}
//
//
//std::vector<glm::vec3> CreateStarField(const unsigned star_count)
//{
//    std::vector<glm::vec3> stars_in_world_position;
//    stars_in_world_position.reserve(star_count);
//
//    for (unsigned i = 0; i < star_count; ++i)
//        stars_in_world_position.emplace_back(random_float<-1, 1>(), random_float<-1, 1>(), random_float<0, 1>());
//
//    return stars_in_world_position;
//}
//
//void MoveStarField(std::vector<glm::vec3>& stars_in_world_position, const glm::vec3& velocity)
//{
//    for (auto& position : stars_in_world_position)
//    {
//        position -= velocity;
//        if (position.z <= 0)
//            position.z = 1;
//    }
//}
//
//void DrawStarField(Window& window, const std::vector<glm::vec3>& stars_in_world_position)
//{
//    int width, height;
//    SDL_GetWindowSize(window.handle, &width, &height);
//
//    const float focal_length = width / 2.0f;
//
//    for (const auto& world_position : stars_in_world_position)
//    {
//        if (world_position.z <= 0)
//            continue;
//
//        const float u = focal_length * (world_position.x / world_position.z) + width  / 2.0f;
//        const float v = focal_length * (world_position.y / world_position.z) + height / 2.0f;
//
//        const glm::vec4 color = (0.2f * WHITE) / (world_position.z * world_position.z);
//
//        const auto min = glm::vec4(0, 0, 0, 1);
//        const auto max = glm::vec4(1, 1, 1, 1);
//
//        DrawPixel(window, glm::uvec2(u, v), glm::clamp(color, min, max));
//    }
//}
//
//
//int main()
//{
//	constexpr int width  = 400;
//	constexpr int height = 400;
//
//
//    InitializeSDL2();
//
//	Window window = CreateWindow("Lab1", width, height);
//
//    Clock clock;
//
//    std::vector<glm::vec3> star_field = CreateStarField(1000);
//    glm::vec3 star_velocity (0.0f, 0.0f, 0.1f);
//    const float slow_star_speed   = 0.05f;
//    const float normal_star_speed = 0.2f;
//    const float fast_star_speed   = 0.5f;
//
//	bool running = true;
//
//	State state = State::RAINBOW;
//
//	while (running)
//	{
//	    const float delta = Tick(clock);
//
//		// --- HANDLE EVENTS ----
//		SDL_Event event;
//		while (SDL_PollEvent(&event))
//		{
//            if (event.type == SDL_QUIT)
//            {
//		        running = false;
//            }
//            else if (event.type == SDL_KEYDOWN)
//            {
//                if (event.key.keysym.sym == SDLK_BACKSPACE)
//                    Clear(window, WHITE);
//                else if (event.key.keysym.sym == SDLK_p)
//                    ScreenShot(window, "../screenshot.png");
//                else if (event.key.keysym.sym == SDLK_w)
//                    star_velocity.z = fast_star_speed;
//                else if (event.key.keysym.sym == SDLK_a)
//                    star_velocity.x = -fast_star_speed;
//                else if (event.key.keysym.sym == SDLK_d)
//                    star_velocity.x = fast_star_speed;
//                else if (event.key.keysym.sym == SDLK_s)
//                    star_velocity.z = slow_star_speed;
//                else if (event.key.keysym.sym == SDLK_SPACE)
//                {
//                    if (state == State::RAINBOW)
//                        state = State::STAR_FIELD;
//                    else if (state == State::STAR_FIELD)
//                        state = State::RAINBOW;
//                }
//            }
//            else if (event.type == SDL_KEYUP)
//            {
//                if (event.key.keysym.sym == SDLK_w)
//                    star_velocity.z = normal_star_speed;
//                else if (event.key.keysym.sym == SDLK_a)
//                    star_velocity.x = 0;
//                else if (event.key.keysym.sym == SDLK_d)
//                    star_velocity.x = 0;
//                else if (event.key.keysym.sym == SDLK_s)
//                    star_velocity.z = normal_star_speed;
//            }
//		}
//
//
//		if (state == State::RAINBOW)
//        {
//            // --- RENDER ----
//            Rainbow(window);
//        }
//        else
//        {
//            // --- UPDATE ----
//            MoveStarField(star_field, star_velocity * delta);
//
//            // --- RENDER ----
//            Clear(window, BLACK);
//            DrawStarField(window, star_field);
//        }
//
//        Render(window);
//	}
//
//
//	// ---- CLEAN UP ----
//    DestroyWindow(window);
//	SDL_Quit();
//}
//
//
//


#include "main.h"

#include <vector>
#include <random>

#include <glm/glm.hpp>


#define MAX_U64 0xFFFFFFFFFFFFFFFF


using glm::vec3;
using glm::vec4;

enum class State { RAINBOW, STAR_FIELD };

static const glm::vec4 WHITE  (1.0f, 1.0f, 1.0f, 1.0f);
static const glm::vec4 RED    (1.0f, 0.0f, 0.0f, 1.0f);
static const glm::vec4 BLUE   (0.0f, 0.0f, 1.0f, 1.0f);
static const glm::vec4 YELLOW (0.0f, 1.0f, 1.0f, 1.0f);
static const glm::vec4 GREEN  (0.0f, 1.0f, 0.0f, 1.0f);


struct GameState
{
	State state;
	vec3  star_velocity;
	float slow_star_speed;
	float normal_star_speed;
	float fast_star_speed;

	Array<vec3>* star_field;
};


static u64 seed = 615132040305;
static u64 XORShift64()
{
	// https://en.wikipedia.org/wiki/Xorshift
	u64 x = seed;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	seed = x;

	return x;
}

static f32 RandomUnilateral()
{
	f32 random  = cast(XORShift64(), f32);
	f32 maximum = cast(MAX_U64, f32);

	f32 result = random / maximum;

	return result;
}

static f32 RandomBilateral()
{
	f32 result = 2.0f * RandomUnilateral() - 1.0f;

	return result;
}


void Clear(FrameBuffer& framebuffer, u8 channel_value)
{
	size_t size = cast(framebuffer.width, size_t) * cast(framebuffer.height, size_t) * sizeof(Pixel);
	memset(framebuffer.pixels, channel_value, size);
}


void Interpolate(Array<vec4>& buffer, const vec4 start, const vec4 stop)
{
	u32 samples = buffer.count;

	if (samples == 1)
	{
		buffer.data[0] = start;
		return;
	}

    const vec4 delta = (stop - start) / cast(samples-1, float);  // Avoiding off-by-one error.
	vec4 value = start;

    for (u32 i = 0; i < samples; ++i)
    {
		buffer.data[i] = value;
        value += delta;
    }
}


void Rainbow(Memory& memory, FrameBuffer& framebuffer)
{
    const vec4 top_left     = RED;
    const vec4 top_right    = BLUE;
    const vec4 bottom_left  = YELLOW;
    const vec4 bottom_right = GREEN;

	Array<vec4>* left_column  = PushStackArray<vec4>(memory, framebuffer.height);
	Array<vec4>* right_column = PushStackArray<vec4>(memory, framebuffer.height);

    Interpolate(*left_column,  top_left,  bottom_left);
    Interpolate(*right_column, top_right, bottom_right);

    for (s32 row = 0; row < framebuffer.height; ++row)
    {
		Array<vec4>* row_colors = PushStackArray<vec4>(memory, framebuffer.width);
        Interpolate(*row_colors, left_column->data[row], right_column->data[row]);

        for (s32 column = 0; column < cast(row_colors->count, s32); ++column)
        {
			vec4 color = row_colors->data[column];

			const auto min = vec4(0, 0, 0, 1);
			const auto max = vec4(1, 1, 1, 1);

			const auto final_color = glm::clamp(color, min, max);

			Pixel& pixel = framebuffer.pixels[row * framebuffer.width + column];
			pixel.r = final_color.r;
			pixel.g = final_color.g;
			pixel.b = final_color.b;
			pixel.a = final_color.a;
        }

		PopStackArray(memory, row_colors);
    }

	PopStackArray(memory, right_column);
	PopStackArray(memory, left_column);
}


Array<vec3>* CreateStarField(Memory& memory, const u32 star_count)
{
	Array<vec3>* star_field = AllocatePersistantArray<vec3>(memory, star_count, 10);
	if (star_field == 0)
		return 0;

	for (u32 i = 0; i < star_field->count; ++i)
	{
		float x = RandomBilateral();
		float y = RandomBilateral();
		float z = RandomUnilateral();

		star_field->data[i] = vec3(x, y, z);
	}

    return star_field;
}


void MoveStarField(Array<vec3>& star_field, vec3 velocity)
{
	for (u32 i = 0; i < star_field.count; ++i)
	{
		vec3& position = star_field.data[i];

        position -= velocity;
        if (position.z <= 0.001)
            position.z = 1;
    }
}


void DrawStarField(FrameBuffer& framebuffer, const Array<vec3>& star_field)
{
    const float focal_length = framebuffer.width / 2.0f;

	for (u32 i = 0; i < star_field.count; ++i)
	{
		vec3& position = star_field.data[i];

        if (position.z <= 0.01 || position.z > 1)
            continue;

        const float u = focal_length * (position.x / position.z) + framebuffer.width  / 2.0f;
        const float v = focal_length * (position.y / position.z) + framebuffer.height / 2.0f;

		s32 col = RoundToS32(u);
		s32 row = RoundToS32(v);

		if (!(0 <= col && col < framebuffer.width && 0 <= row && row < framebuffer.height))
			continue;

		u32 index = cast(row * framebuffer.width + col, u32);

		// Fade the further the stars are.
        const vec4 color = (0.2f * WHITE) / (position.z * position.z);

        const auto min = vec4(0, 0, 0, 1);
        const auto max = vec4(1, 1, 1, 1);

		const auto final_color = glm::clamp(color, min, max);

		Pixel& pixel = framebuffer.pixels[index];
		pixel.r = final_color.r;
		pixel.g = final_color.g;
		pixel.b = final_color.b;
		pixel.a = final_color.a;
    }
}



void Initialize(Memory& memory)
{
	if (!memory.initialized)
	{
		GameState* state = AllocatePersistantObject<GameState>(memory);

		state->state = State::STAR_FIELD;
		state->star_velocity	 = vec3(0.0f, 0.0f, 0.1f);
		state->slow_star_speed   = 0.05f;
		state->normal_star_speed = 0.2f;
		state->fast_star_speed   = 0.5f;
		
		state->star_field = CreateStarField(memory, 10000);

		memory.initialized = true;
	}
}


void Update(Memory& memory, FrameBuffer& framebuffer, Keyboard& keyboard, f32 dt)
{
	GameState* state = reinterpret_cast<GameState*>(memory.persistent.data);

	for (int i = 0; i < keyboard.used; ++i)
	{
		Key key = keyboard.keys[i];

        if (key.ended_on_down)
        {
            if (key.character == 'Q')
                Clear(framebuffer, 255);
            else if (key.character == 'P')
                Screenshot(framebuffer, "screenshots/lab1/screenshot.png");
            else if (key.character == 'W')
				state->star_velocity.z = state->fast_star_speed;
            else if (key.character == 'A')
				state->star_velocity.x = -state->fast_star_speed;
            else if (key.character == 'D')
				state->star_velocity.x = state->fast_star_speed;
            else if (key.character == 'S')
				state->star_velocity.z = state->slow_star_speed;
            else if (key.character == ' ')
            {
                if (state->state == State::RAINBOW)
					state->state = State::STAR_FIELD;
                else if (state->state == State::STAR_FIELD)
					state->state = State::RAINBOW;
            }
        }
        else
        {
            if (key.character == 'W')
				state->star_velocity.z = state->normal_star_speed;
            else if (key.character == 'A')
				state->star_velocity.x = 0;
            else if (key.character == 'D')
				state->star_velocity.x = 0;
            else if (key.character == 'S')
				state->star_velocity.z = state->normal_star_speed;
        }
	}

	if (state->state == State::RAINBOW)
	{
	    Rainbow(memory, framebuffer);
	}
	else
	{
	    // --- UPDATE ----
	    MoveStarField(*state->star_field, state->star_velocity * dt);
	
	    // --- RENDER ----
	    Clear(framebuffer, 0);
	    DrawStarField(framebuffer, *state->star_field);
	}
}


void Sound(Memory& memory, SoundBuffer& buffer)
{
	
}

