#pragma once

// These are shared across all platforms.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

// FIX(ted): THIS PRESUMABLY ONLY WORKS ON UNIX. FIX!
#include <signal.h>    // raise(SIGINT)

#define MAX_U8  0xFF
#define MAX_U16 0xFFFF
#define MAX_U32 0xFFFFFFFF
#define MAX_U64 0xFFFFFFFFFFFFFFFF



#define PI32 3.1415927410125732421875f

#define cast(x, type) static_cast<type>(x)

#define ASSERT(condition, ...) do { if (!(condition)) exit(2); } while(0)

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


inline u16 RoundToU16(f32 x)
{
	ASSERT(0 <= x && x <= MAX_U16);
	u16 result = cast(x + 0.5f, u16);
	return result;
}
inline u16 FloorToU16(f32 x)
{
	ASSERT(0 <= x && x <= MAX_U16);
	u16 result = cast(x, u16);
	return result;
}
inline u32 RoundToU32(f32 x)
{
	ASSERT(0 <= x && x <= MAX_U32);
	u32 result = cast(x + 0.5f, u32);
	return result;
}
inline u64 RoundToU64(f32 x)
{
	ASSERT(0 <= x && x <= MAX_U64);
	u64 result = cast(x + 0.5f, u64);
	return result;
}

inline u32 RoundToS32(f32 x)
{
	s32 result = (x >= 0) ? cast(x + 0.5f, s32) : cast(x - 0.5f, s32);
	return result;
}


struct Buffer
{
	u32 size;
	u32 used;
	u8*	data;
};

template <typename T = u8>
struct Array
{
	u32 count;
	T*  data;
};

template <typename T = u8>
struct Array2D
{
	u16 rows;
	u16 columns;
	T*  data;

	T& operator() (u32 row, u32 column) { return data[row * rows + column]; }
};


struct Memory;

typedef bool	  (*PlatformWriteAsset) (const char* name, const Array<Array<u8>> buffers);
typedef Array<u8> (*PlatformReadAsset)  (const char* name, Memory& memory);

struct Memory
{
	Buffer	  persistent;
	Buffer    temporary;
	uintptr_t stack_reference;
	bool	  initialized;

	PlatformWriteAsset WriteAsset;
	PlatformReadAsset  ReadAsset;
};

inline bool HasOverflown(u32 a, u32 b, u32 result)
{
	return (result < a || result < b);
}

template <typename T = u8>
Array<T> AllocateArray(Buffer& memory, u32 count, u32 decrement_if_fail = 0, u32 minimum_allowed_count = 1)
{
	Array<T> array;

	u32 total_size = sizeof(T) * count;
	ASSERT(!HasOverflown(sizeof(T), count, total_size));

	u32 used_memory_after_allocation = memory.used + total_size;
	while (used_memory_after_allocation >= memory.size || HasOverflown(memory.used, total_size, used_memory_after_allocation))
	{
		if (count < minimum_allowed_count || decrement_if_fail == 0)
		{
			array.count = 0;
			array.data  = 0;
			return array;
		}

		count -= decrement_if_fail;
		total_size = sizeof(T) * count;
		used_memory_after_allocation = memory.used + total_size;
	}

	array.count = count;
	array.data  = reinterpret_cast<T*>(memory.data + memory.used);

	memory.used = used_memory_after_allocation;

	return array;
}
template <typename T>
T* AllocateObject(Buffer& memory)
{
	u32 size = sizeof(T);

	u32 used_memory_after_allocation = memory.used + size;
	if (used_memory_after_allocation >= memory.size || HasOverflown(memory.used, size, used_memory_after_allocation))
		return 0;

	T* object = reinterpret_cast<T*>(memory.data);

	memory.used = used_memory_after_allocation;

	return object;
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

	Pixel(f32 r, f32 g, f32 b, f32 a) : r(r), g(g), b(b), a(a) {}

	Pixel& operator= (const Pixel& other) { r = other.r; g = other.g; b = other.b; a = other.a; return *this;  }

	Pixel operator* (f32 factor)		 const { return Pixel(r * factor,  g * factor,  b * factor,  a * factor);  }
	Pixel operator+ (const Pixel& other) const { return Pixel(r + other.r, g + other.g, b + other.b, a + other.a); }
	Pixel operator- (const Pixel& other) const { return Pixel(r - other.r, g - other.g, b - other.b, a - other.a); }
};

struct FrameBuffer
{
	u16    width;
	u16    height;
	Pixel* pixels;

	Pixel& operator() (u16 row, u16 column) { return pixels[row * width + column]; }
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
	// Clamp.
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

Pixel UnPackRGBA(u32 data)
{
	f32 r = cast((data >> 0)  & 0x000000FF, f32);
	f32 g = cast((data >> 8)  & 0x000000FF, f32);
	f32 b = cast((data >> 16) & 0x000000FF, f32);
	f32 a = cast((data >> 24) & 0x000000FF, f32);

	Pixel pixel = Pixel(r, g, b, a);
	return pixel;
}

u32 PackBGRA(f32 r, f32 g, f32 b, f32 a)
{
	u32 result = (RoundToU32(a) << 24) |
				 (RoundToU32(r) << 16) |
				 (RoundToU32(g) << 8)  |
				 (RoundToU32(b) << 0);

	return result;
}

Pixel UnPackBGRA(u32 data)
{
	f32 b = cast((data >> 0) & 0x000000FF, f32);
	f32 g = cast((data >> 8) & 0x000000FF, f32);
	f32 r = cast((data >> 16) & 0x000000FF, f32);
	f32 a = cast((data >> 24) & 0x000000FF, f32);

	Pixel pixel = Pixel(r, g, b, a);
	return pixel;
}

void WriteImage(Memory memory, FrameBuffer framebuffer, const char* file_name)
{
	u32 size_of_image = framebuffer.width * framebuffer.height * sizeof(u32);

	BitmapHeader header = {};

	header.file_type = 0x4D42;
	header.file_size = sizeof(header) + size_of_image;
	header.bitmap_offset = sizeof(header);
	header.size = sizeof(header) - 14;
	header.width = framebuffer.width;
	header.height = -framebuffer.height;  // Negative numbers gives a direction of top-down instead of bottom-up.
	header.planes = 1;
	header.bits_per_pixel = 32;
	header.compression = 0;
	header.size_of_bitmap = size_of_image;
	header.horizontal_resolution = 0;
	header.vertical_resolution = 0;
	header.colors_used = 0;
	header.colors_important = 0;

	Array<u8> meta_data  = { sizeof(header), reinterpret_cast<u8*>(&header) };
	Array<u8> image_data = AllocateArray<u8>(memory.temporary, size_of_image);

	u8* base = image_data.data;

	for (int i = 0; i < framebuffer.width * framebuffer.height; ++i)
	{
		Pixel& color = framebuffer.pixels[i];
		u32 formatted_color = PackBGRA(color.r * 255.0f, color.g * 255.0f, color.b * 255.0f, color.a * 255.0f);
		memcpy(base, &formatted_color, sizeof(formatted_color));
		base += sizeof(formatted_color);
	}

	Array<u8> temp[] = { meta_data, image_data };

	Array<Array<u8>> buffers;
	buffers.count = 2;
	buffers.data = temp;

	memory.WriteAsset(file_name, buffers);
}

FrameBuffer ReadImage(Memory memory, const char* name)
{
	Array<u8> asset = memory.ReadAsset(name, memory);
	u8* base = asset.data;

	BitmapHeader* header = reinterpret_cast<BitmapHeader*>(base);
	base += sizeof(BitmapHeader);

	FrameBuffer framebuffer;
	framebuffer.width  = cast(header->width, u16);
	framebuffer.height = (header->height > 0) ? cast(header->height, u16) : cast(-header->height, u16);
	framebuffer.pixels = AllocateArray<Pixel>(memory.persistent, framebuffer.width * framebuffer.height).data;

	for (int i = 0; i < framebuffer.width * framebuffer.height; ++i)
	{
		u32* formatted_color = reinterpret_cast<u32*>(base);
		framebuffer.pixels[i] = UnPackBGRA(*formatted_color) * (1 / 255.0f);
		base += sizeof(u32);
	}

	return framebuffer;
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

