#pragma once

// These are shared across all platforms.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

// FIX(ted): THIS PRESUMABLY ONLY WORKS ON UNIX. FIX!
#include <signal.h>    // raise(SIGINT)


#define PI32 3.1415927410125732421875f


#define cast(x, type) static_cast<type>(x)

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;

// Memory
#define KILOBYTES(x) (         (x) * 1024ULL)
#define MEGABYTES(x) (KILOBYTES(x) * 1024ULL)
#define GIGABYTES(x) (MEGABYTES(x) * 1024ULL)
#define TERABYTES(x) (GIGABYTES(x) * 1024ULL)


u32 RoundToU32(f32 x)
{
	u32 result = cast(x + 0.5f, u32);
	return result;
}

u32 RoundToS32(f32 x)
{
	s32 result = cast(x + 0.5f, s32);
	return result;
}


struct Buffer
{
	u32 count;  // Maximum 4GB
	u32 used;
	u8* data;
};

template <typename T = u8>
struct Array
{
	u32 count;
	T*  data;
};


struct Memory
{
	Buffer   persistent;
	Buffer   temporary;
	intptr_t stack_reference;
	bool	 initialized;
};

template <typename T = u8>
Array<T>* AllocatePersistantArray(Memory& memory, u32 count, u32 decrement_if_fail = 0, u32 minimum_allowed_count = 0)
{
	u64 total_size = sizeof(Array<T>) + sizeof(T) * count;

	while (memory.persistent.used + total_size >= memory.persistent.count || memory.persistent.used + total_size < memory.persistent.used /* Overflow */)
	{
		if (count <= minimum_allowed_count || decrement_if_fail == 0)
			return 0;

		count -= decrement_if_fail;
		total_size = sizeof(Array<T>) + sizeof(T) * count;
	}

	Array<T>* buffer = reinterpret_cast<Array<T>*>(memory.persistent.data + memory.persistent.used);
	
	buffer->count = count;
	buffer->data  = reinterpret_cast<T*>(buffer + 1);

	memory.persistent.used += cast(total_size, u32);

	return buffer;
}
template <typename T>
T* AllocatePersistantObject(Memory& memory)
{
	size_t size = sizeof(T);

	if (memory.persistent.used + size >= memory.persistent.count || memory.persistent.used + size < memory.persistent.used /* Overflow */)
		return 0;

	T* data = reinterpret_cast<T*>(memory.persistent.data + memory.persistent.used);
	memory.persistent.used += size;

	return data;
}

template <typename T = u8>
Array<T>* AllocateTemporaryArray(Memory & memory, u32 count, u32 decrement_if_fail = 0, u32 minimum_allowed_count = 0)
{
	u64 total_size = sizeof(Array<T>) + sizeof(T) * count;

	while (memory.temporary.used + total_size >= memory.temporary.size || memory.temporary.used + total_size < memory.temporary.used /* Overflow */)
	{
		if (count <= minimum_allowed_count || decrement_if_fail == 0)
			return 0;

		count -= decrement_if_fail;
		total_size = sizeof(Array<T>) + sizeof(T) * count;
	}

	Array<T>* buffer = reinterpret_cast<Array<T>*>(memory.temporary.data + memory.temporary.used);

	buffer->count = count;
	buffer->data  = reinterpret_cast<T*>(buffer + 1);

	memory.temporary.used += cast(total_size, u32);

	return buffer;
}
template <typename T>
T* AllocateTemporaryObject(Memory& memory)
{
	size_t size = sizeof(T);

	if (memory.temporary.used + size >= memory.temporary.count || memory.temporary.used + size < memory.temporary.used /* Overflow */)
		return 0;

	T* data = reinterpret_cast<T*>(memory.temporary.data + memory.temporary.used);
	memory.temporary.used += size;

	return data;
}
template <typename T = u8>
Array<T>* PushStackArray(Memory& memory, u32 count, u32 decrement_if_fail = 0, u32 minimum_allowed_count = 0)
{
	u64 total_size = sizeof(Array<T>) + sizeof(T) * count;

	while (memory.temporary.used + total_size >= memory.temporary.count || memory.temporary.used + total_size < memory.temporary.used /* Overflow */)
	{
		if (count <= minimum_allowed_count || decrement_if_fail == 0)
			return 0;

		count -= decrement_if_fail;
		total_size = sizeof(Array<T>) + sizeof(T) * count;
	}

	Array<T>* buffer = reinterpret_cast<Array<T>*>(memory.temporary.data + memory.temporary.used);

	buffer->count = count;
	buffer->data  = reinterpret_cast<T*>(buffer + 1);

	memory.temporary.used += cast(total_size, u32);
	memory.stack_reference = reinterpret_cast<intptr_t>(buffer) + cast(total_size, intptr_t);

	return buffer;
}
template <typename T = u8>
void PopStackArray(Memory& memory, Array<T>* buffer)
{
	u64 total_size = sizeof(Array<T>) + sizeof(T) * buffer->count;

	if (memory.stack_reference - cast(total_size, intptr_t) != reinterpret_cast<intptr_t>(buffer))
	{
		// TODO(ted): Add logging.
		exit(13);
	}

	memory.temporary.used -= cast(total_size, u32);
	memory.stack_reference = reinterpret_cast<intptr_t>(memory.temporary.data + memory.temporary.used);
}


struct Pixel
{
#if defined(__APPLE__) && defined(__MACH__)
	f32 r, g, b, a;
#elif defined(_WIN32) || defined(_WIN64)
	f32 b, g, r, a;
#else
	#error "Your operating system is not supported."
#endif
};

struct FrameBuffer
{
	s32 width;
	s32 height;
	Pixel* pixels;
};

#pragma pack(push, 1)
struct BitmapHeader
{
	u16 file_type;
	u32 file_size;
	u16 reserved_1;
	u16 reserved_2;
	u32 bitmap_offset;
	u32 size;
	s32 width;
	s32 height;  // Negative numbers gives a direction of top-down instead of bottom-up.
	u16 planes;
	u16 bits_per_pixel;
	u32 compression;
	u32 size_of_bitmap;
	s32 horizontal_resolution;
	s32 vertical_resolution;
	u32 colors_used;
	u32 colors_important;
};
#pragma pack(pop)

f32 ExactLinearTosRGB(f32 L)
{
	if (L < 0.0f)
		L = 0.0f;
	else if (L > 1.0f)
		L = 1.0f;

	f32 S = L * 12.92f;
	if (L > 0.0031308f)	
		S = 1.055f * powf(L, 1.0f / 2.4f) - 0.055f;

	return S;
}

u32 PackRGBA(f32 r, f32 g, f32 b, f32 a)
{
	u32 result = (RoundToU32(a) << 24) |
			     (RoundToU32(b) << 16) |
			     (RoundToU32(g) << 8)  |
			     (RoundToU32(r) << 0);

	return result;
}

u32 PackBGRA(f32 r, f32 g, f32 b, f32 a)
{
	u32 result = (RoundToU32(a) << 24) |
				 (RoundToU32(r) << 16) |
				 (RoundToU32(g) << 8) |
				 (RoundToU32(b) << 0);

	return result;
}

bool Screenshot(FrameBuffer buffer, const char* file_name)
{
	u32 size_of_image = buffer.width * buffer.height * sizeof(Pixel);

	BitmapHeader header = {};

	header.file_type = 0x4D42;
	header.file_size = sizeof(header) + size_of_image;
	header.bitmap_offset = sizeof(header);
	header.size = sizeof(header) - 14;
	header.width = buffer.width;
	header.height = -buffer.height;  // Negative numbers gives a direction of top-down instead of bottom-up.
	header.planes = 1;
	header.bits_per_pixel = 32;
	header.compression = 0;
	header.size_of_bitmap = size_of_image;
	header.horizontal_resolution = 0;
	header.vertical_resolution = 0;
	header.colors_used = 0;
	header.colors_important = 0;

	u32* colors = cast(malloc(size_of_image), u32*);
	for (int i = 0; i < buffer.width * buffer.height; ++i)
	{
		Pixel& color = buffer.pixels[i];
		colors[i] = PackBGRA(
			ExactLinearTosRGB(color.r) * 255.0f,
			ExactLinearTosRGB(color.g) * 255.0f,
			ExactLinearTosRGB(color.b) * 255.0f,
			ExactLinearTosRGB(color.a) * 255.0f
		);
	}

	FILE* file = fopen(file_name, "wb");
	if (file)
	{
		fwrite(&header, sizeof(header), 1, file);
		fwrite(colors, size_of_image, 1, file);
		fclose(file);
		return 1;
	}
	else
	{
		fprintf(stderr, "[Error]: Unable to open output file.\n");
		return 0;
	}
}


struct Sample { s16 left; s16 right; };
struct SoundBuffer
{
	u32  size;
	s16* data;
};


struct Key
{
	s8   character;
	s32  transitions;
	bool ended_on_down;
};

struct Keyboard
{
	u16 used;
	Key keys[128];
};


// https://sourceforge.net/p/predef/wiki/OperatingSystems/

// EXPORT_FUNCTION(name, parameters):
//    1. Forward declares the function with 'extern "C"' and as DLL.
//    2. Type defines the function as "<name>Function".
//    3. Creates an empty stub function called "DEFAULT_<name>.

#if defined(__APPLE__) && defined(__MACH__)
#define EXPORT_FUNCTION(name, ...)                                          \
extern "C" __attribute__((visibility("default"))) void name(__VA_ARGS__);   \
typedef void (*name##Function)(__VA_ARGS__);                                \
void DEFAULT_##name(__VA_ARGS__) {}                                         \

#elif defined(_WIN32) || defined(_WIN64)
#define EXPORT_FUNCTION(name, ...)                                          \
extern "C" __declspec(dllexport) void name(__VA_ARGS__);                    \
typedef void (*name##Function)(__VA_ARGS__);                                \
void DEFAULT_##name(__VA_ARGS__) {}                                         \

#elif defined(__linux__)
#error "Don't support Linux yet."

#else
#error "Couldn't determine operating system!"
#endif


EXPORT_FUNCTION(Initialize, Memory&);
EXPORT_FUNCTION(Update, Memory&, FrameBuffer&, Keyboard&, f32);
EXPORT_FUNCTION(Sound, Memory&, SoundBuffer&);

