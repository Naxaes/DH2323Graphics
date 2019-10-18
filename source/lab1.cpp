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

void Interpolate2D(Array2D<vec4>& buffer, const vec4 topleft, const vec4 bottomleft, const vec4 topright, const vec4 bottomright)
{
	u32 y_samples = buffer.rows;
	u32 x_samples = buffer.columns;

	u32 first_x = 0;
	u32 last_x  = buffer.columns - 1;

	// INTERPOLATE LEFT COLUMN
	{
		if (y_samples == 1)
		{
			buffer.data[first_x] = topleft;
			return;
		}

		const vec4 delta = (bottomleft - topleft) / cast(y_samples - 1, f32);  // Avoiding off-by-one error.
		vec4 value = topleft;

		for (u32 i = 0; i < y_samples; ++i)
		{
			buffer.data[i * buffer.columns + first_x] = value;
			value += delta;
		}
	}

	// INTERPOLATE RIGHT COLUMN
	{
		if (y_samples == 1)
		{
			buffer.data[last_x] = topright;
			return;
		}

		const vec4 delta = (bottomright - topright) / cast(y_samples - 1, f32);  // Avoiding off-by-one error.
		vec4 value = topright;

		for (u32 i = 0; i < y_samples; ++i)
		{
			buffer.data[i * buffer.columns + last_x] = value;
			value += delta;
		}
	}

	// INTERPOLATE ROWS
	{
		if (x_samples == 1)
		{
			return;
		}

		for (u32 row = 0; row < y_samples; ++row)
		{
			vec4 left  = buffer.data[row * buffer.columns + first_x];
			vec4 right = buffer.data[row * buffer.columns + last_x];

			const vec4 delta = (right - left) / cast(x_samples - 1, f32);  // Avoiding off-by-one error.
			vec4 value = left + delta;

			for (u32 column = first_x + 1; column < last_x; ++column)
			{
				buffer.data[row * buffer.rows + column] = value;
				value += delta;
			}
		}
	}
}


void Rainbow(Memory& memory, FrameBuffer& framebuffer)
{
	Array<vec4>* left_column  = PushStackArray<vec4>(memory, framebuffer.height);
	Array<vec4>* right_column = PushStackArray<vec4>(memory, framebuffer.height);
	Array<vec4>* row_colors   = PushStackArray<vec4>(memory, framebuffer.width);

    Interpolate(*left_column,  RED,  YELLOW);
    Interpolate(*right_column, BLUE, GREEN);

    for (s32 row = 0; row < framebuffer.height; ++row)
    {
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

    }

	PopStackArray(memory, row_colors);
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
