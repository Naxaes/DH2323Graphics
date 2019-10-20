#include "main.h"

#include <vector>
#include <random>

#include <glm/glm.hpp>


using glm::vec3;
using glm::vec4;

enum class State { RAINBOW, STAR_FIELD };

static const Pixel WHITE  (1.0f, 1.0f, 1.0f, 1.0f);
static const Pixel RED    (1.0f, 0.0f, 0.0f, 1.0f);
static const Pixel BLUE   (0.0f, 0.0f, 1.0f, 1.0f);
static const Pixel YELLOW (0.0f, 1.0f, 1.0f, 1.0f);
static const Pixel GREEN  (0.0f, 1.0f, 0.0f, 1.0f);


struct GameState
{
	State state;
	vec3  star_velocity;
	float slow_star_speed;
	float normal_star_speed;
	float fast_star_speed;

	Array<vec3> star_field;
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


void Clear(FrameBuffer& framebuffer, vec3 background)
{
	Pixel pixel = { background.r, background.g, background.b, 1.0f };
	for (s32 row = 0; row < framebuffer.height; row++)
	{
		for (s32 column = 0; column < framebuffer.width; column++)
		{
			framebuffer.pixels[row * framebuffer.width + column] = pixel;
		}
	}
}


void Interpolate(Array<vec4>& buffer, const vec4 start, const vec4 stop)
{
	u32 samples = buffer.count;
	
	if (samples <= 0)
		return;

    for (u32 i = 0; i < samples; ++i)
    {
		f32 factor = cast(i, f32) / cast(samples-1, f32);
		buffer.data[i] = start * (1 - factor)  +  stop * factor;
    }
}

void Interpolate2D(FrameBuffer& framebuffer, const Pixel topleft, const Pixel bottomleft, const Pixel topright, const Pixel bottomright)
{
	u16 y_samples = framebuffer.height;
	u16 x_samples = framebuffer.width;

	if (y_samples <= 1 || x_samples <= 1)
		return;

	u16 last_x = framebuffer.width - 1;

	// INTERPOLATE LEFT COLUMN
	for (u16 row = 0; row < y_samples; ++row)
	{
		f32 factor = cast(row, f32) / cast(y_samples - 1, f32);
		framebuffer(row, 0) = topleft * (1 - factor) + bottomleft * factor;
	}

	// INTERPOLATE RIGHT COLUMN
	for (u16 row = 0; row < y_samples; ++row)
	{
		f32 factor = cast(row, f32) / cast(y_samples - 1, f32);
		framebuffer(row, last_x) = topright * (1 - factor) + bottomright * factor;
	}

	// INTERPOLATE ROWS
	for (u16 row = 0; row < y_samples; ++row)
	{
		for (u16 column = 1; column < x_samples-1; column++)
		{
			f32 factor = cast(column, f32) / cast(x_samples - 1, f32);
			framebuffer(row, column) = framebuffer(row, 0) * (1 - factor) + framebuffer(row, last_x) * factor;
		}	
	}
}


void Rainbow(FrameBuffer& framebuffer)
{
	Interpolate2D(framebuffer, RED, GREEN, BLUE, YELLOW);
}


Array<vec3> CreateStarField(Memory& memory, const u32 star_count)
{
	Array<vec3> star_field = AllocateArray<vec3>(memory.persistent, star_count, 10);

	for (u32 i = 0; i < star_field.count; ++i)
	{
		float x = RandomBilateral();
		float y = RandomBilateral();
		float z = RandomUnilateral();

		star_field.data[i] = vec3(x, y, z);
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

		if (!(0 <= u && u < framebuffer.width && 0 <= v && v < framebuffer.height))
			continue;

		u16 column = FloorToU16(u);
		u16 row	   = FloorToU16(v);

		// Fade the further the stars are.
        const vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f) * (1.0f - position.z);

		Pixel& pixel = framebuffer(row, column);
		pixel.r = color.r;
		pixel.g = color.g;
		pixel.b = color.b;
		pixel.a = color.a;
    }
}



void Initialize(Memory& memory)
{
	if (!memory.initialized)
	{
		GameState* state = AllocateObject<GameState>(memory.persistent);

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
                Clear(framebuffer, vec3(0.0f, 0.0f, 0.0f));
            else if (key.character == 'P')
                Screenshot(framebuffer, "../screenshots/lab1/screenshot.png");
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
	    Rainbow(framebuffer);
	}
	else
	{
	    // --- UPDATE ----
	    MoveStarField(state->star_field, state->star_velocity * dt);
	
	    // --- RENDER ----
		Clear(framebuffer, vec3(0.0f, 0.0f, 0.0f));
	    DrawStarField(framebuffer, state->star_field);
	}
}


void Sound(Memory& memory, SoundBuffer& buffer)
{
	
}
