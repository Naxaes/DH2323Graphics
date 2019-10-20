#include "main.h"

#include <windows.h>
#include <xinput.h>
#include <dsound.h>

#define StringLiteral(x)  {x, sizeof(x) - 1};


/*
Directory structure:
    root
	|- build
	|- data
	|- includes
	|- source
*/


#define WIN32_REPORT_ERROR(...)                                                                         \
do {                                                                                                    \
    char buffer1[255] = { 0 };                                                                          \
	char buffer2[255] = { 0 };                                                                          \
    sprintf(buffer1, __VA_ARGS__);                                                                      \
	sprintf(buffer2, "[Error %d]:\n\tFile: %s\n\tLine: %i\n\tMessage: %s", GetLastError(), __FILE__, __LINE__, buffer1);   \
    OutputDebugStringA(buffer2);																		\
} while(0)

#define WIN32_ASSERT(condition, ...)			\
do {											\
	if (!(condition))							\
	{											\
		WIN32_REPORT_ERROR(__VA_ARGS__);		\
		exit(-1);								\
	}											\
} while(0)


#include "win32_clock.cpp"




struct Win32FrameBuffer
{
	BITMAPINFO info;

	int width;
	int height;

	int bytes_per_pixel;

	u32* pixels;
};

struct Win32Game
{
	HMODULE  dll_handle;
	FILETIME time_loaded;
	char*	 source_dll;
	char*	 runtime_dll;

	InitializeFunction initialize;
	UpdateFunction     update;
	SoundFunction      sound;
};

static Win32FrameBuffer win32_framebuffer;
static FrameBuffer game_framebuffer;
static bool running;



void BubbleSort(u64* array, u64 count)
{
	for (u64 i = 0; i < count; ++i)
	{
		for (u64 j = i + 1; j < count; ++j)
		{
			if (array[i] > array[j])
			{
				u64 temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}
}


// TODO(ted): Pre-allocate the maximum size at startup. The user should always be able
// to resize to the maximum size, so if we pre-allocate we know that the game won't
// crash at resize due to memory. It'll also simplify the code and make it more efficient.
static void Win32ResizeFrameBuffer(Win32FrameBuffer& win32_buffer, FrameBuffer& framebuffer, u16 width, u16 height)
{
	if (win32_buffer.pixels)
		VirtualFree(win32_buffer.pixels, 0, MEM_RELEASE);
	if (framebuffer.pixels)
		VirtualFree(framebuffer.pixels, 0, MEM_RELEASE);

	win32_buffer.info.bmiHeader.biSize   = sizeof(win32_buffer.info.bmiHeader);
	win32_buffer.info.bmiHeader.biWidth  = width;
	win32_buffer.info.bmiHeader.biHeight = -height;  // Negative values makes the bitmap top-down, instead of bottom-up.
	win32_buffer.info.bmiHeader.biPlanes = 1;
	win32_buffer.info.bmiHeader.biBitCount	= 32;
	win32_buffer.info.bmiHeader.biCompression = BI_RGB;
	win32_buffer.info.bmiHeader.biSizeImage	= 0;
	win32_buffer.info.bmiHeader.biXPelsPerMeter = 0;
	win32_buffer.info.bmiHeader.biYPelsPerMeter = 0;
	win32_buffer.info.bmiHeader.biClrUsed = 0;
	win32_buffer.info.bmiHeader.biClrImportant = 0;

	win32_buffer.width  = width;
	win32_buffer.height = height;
	win32_buffer.bytes_per_pixel = 4;
	int win32_buffer_memory_size = win32_buffer.width * win32_buffer.height * win32_buffer.bytes_per_pixel;
	win32_buffer.pixels = cast(VirtualAlloc(0, win32_buffer_memory_size, MEM_COMMIT, PAGE_READWRITE), u32*);

	framebuffer.width  = width;
	framebuffer.height = height;
	int framebuffer_memory_size = framebuffer.width * framebuffer.height * sizeof(Pixel);
	framebuffer.pixels = cast(VirtualAlloc(0, framebuffer_memory_size, MEM_COMMIT, PAGE_READWRITE), Pixel*);
}


static void Win32UpdateWindow(HDC device_context, Win32FrameBuffer& win32_buffer, RECT area)
{
	u32 x = area.left;
	u32 y = area.top;
	u32 width  = area.right  - area.left;
	u32 height = area.bottom - area.top;

	int status = StretchDIBits(
		device_context,
		x, y, width, height,   // Destination
		x, y, width, height,   // Source
		win32_buffer.pixels, &win32_buffer.info,
		DIB_RGB_COLORS, SRCCOPY
	);

	if (status == 0)
	{
		WIN32_REPORT_ERROR("Rows rendered: %d\n", status);
	}
}


static LRESULT CALLBACK Win32EventCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (message)
	{
		case WM_CHAR:
		{
			// Key& key = keyboard.keys[keyboard.used++];
			// key.character = cast(wParam, s8);
		} break;
		case WM_PAINT:  // We repaint continuously.
		{

			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			
			RECT dirty_rect = paint.rcPaint;
			Win32UpdateWindow(device_context, win32_framebuffer, dirty_rect);
	
			EndPaint(window, &paint);
		} break;
		case WM_SIZE:
		{
			RECT client_rect;
			GetClientRect(window, &client_rect);
			u16 width  = cast(client_rect.right  - client_rect.left, u16);
			u16 height = cast(client_rect.bottom - client_rect.top,  u16);

			Win32ResizeFrameBuffer(win32_framebuffer, game_framebuffer, width, height);
		} break;
		case WM_CLOSE:
		{
			if (MessageBoxA(window, "Really quit?", "My application", MB_OKCANCEL) == IDOK)
			{
				DestroyWindow(window);
				PostQuitMessage(0);
			}
		} break;
		case WM_ACTIVATEAPP:
		{

		} break;
		default:
		{
			result = DefWindowProc(window, message, wParam, lParam);
		}
	}

	return result;
}


static void Win32ProcessEvents(Keyboard& keyboard)
{
	keyboard = { 0 };

	MSG message = {};
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		switch (message.message)
		{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			u32  code = (u32) message.wParam;
			// bool was_down = (message.lParam & (1 << 30)) != 0;
			// bool is_down  = (message.lParam & (1 << 31)) != 0;
			// bool alt_down = (message.lParam & (1 << 29)) != 0;

			// TODO(ted): FIX
			Key& key = keyboard.keys[keyboard.used++];
			key.character	  = code < 128 ? cast(code, s8) : 0;  // Incorrect
			key.transitions   = 0;
			key.ended_on_down = true;
		} break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			u32  code = (u32) message.wParam;
			// bool was_down = (message.lParam & (1 << 30)) != 0;
			// bool is_down  = (message.lParam & (1 << 31)) != 0;
			// bool alt_down = (message.lParam & (1 << 29)) != 0;

			// TODO(ted): FIX
			Key& key = keyboard.keys[keyboard.used++];
			key.character	  = code < 128 ? cast(code, s8) : 0;  // Incorrect
			key.transitions   = 0;
			key.ended_on_down = false;
		} break;
		case WM_QUIT:
		{
			running = false;
		} break;
		default:
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
}


// Returns the index of the null terminator.
static u32 Win32GetExecutableDirectory(char* path, size_t size)
{
	// @TODO(ted): size should be in TCHAR's.
	DWORD length = GetModuleFileNameA(NULL, path, size);

	for (int i = length; i >= 0; --i)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = '\0';
			return i + 1;
		}
	}
	
	path[0] = '\0';
	return 0;
}


// Returns the index of the null terminator.
static u32 Win32GetDataDirectory(char* path, size_t size)
{
	// @TODO(ted): size should be in TCHAR's.
	DWORD length = GetModuleFileNameA(NULL, path, size);

	for (int i = length; i >= 0; --i)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = '\0';
			return i + 1;
		}
	}

	path[0] = '\0';
	return 0;
}


static FILETIME Win32GetLastFileWriteTime(const char* filename)
{
	FILETIME write_time = { 0 };

	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFileA(filename, &data);

	if (handle != INVALID_HANDLE_VALUE)
	{
		write_time = data.ftLastWriteTime;
		FindClose(handle);
	}

	return write_time;
}


static void Win32LoadGame(Win32Game& game)
{
	// Open the DLL to see if it's locked or not. If it is, try again later. TOOD(ted): set 0 instead of GENERIC_READ?
	HANDLE dll_file = CreateFileA(game.source_dll, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (dll_file == INVALID_HANDLE_VALUE)
		return;

	// Unload the currently loaded DLL.
	if (game.dll_handle)
		WIN32_ASSERT(FreeLibrary(game.dll_handle) != 0, "Unable to free dll library.\n");

	// Copy the file so the 'source' file is still available for editing whem game is running.
	WIN32_ASSERT(CopyFile(game.source_dll, game.runtime_dll, FALSE) != 0, "Copy failed.\n");

	// Load the copy DLL.
	game.dll_handle = LoadLibrary(game.runtime_dll);
	
	FILETIME ftCreate = { 0 };
	FILETIME ftAccess = { 0 };
	FILETIME ftWrite  = { 0 };

	GetFileTime(dll_file, &ftCreate, &ftAccess, &ftWrite);
	CloseHandle(dll_file);

	game.time_loaded = ftWrite;

	// Initialize all functions from DLL or by a default stub function.
	game.initialize = reinterpret_cast<InitializeFunction>(GetProcAddress(game.dll_handle, "Initialize"));
	if (!game.initialize)
		game.initialize = DEFAULT_Initialize;

	game.update = reinterpret_cast<UpdateFunction>(GetProcAddress(game.dll_handle, "Update"));
	if (!game.update)
		game.update = DEFAULT_Update;

	game.sound = reinterpret_cast<SoundFunction>(GetProcAddress(game.dll_handle, "Sound"));
	if (!game.sound)
		game.sound = DEFAULT_Sound;

	if (!game.initialize || !game.update || !game.sound)
		WIN32_REPORT_ERROR("Couldn't load all game functions!\n");
}


static void Win32HotloadGame(Win32Game& game, const char* source, char* destination)
{
	FILETIME write_time = Win32GetLastFileWriteTime(game.source_dll);
	if (CompareFileTime(&write_time, &game.time_loaded) != 0)
		Win32LoadGame(game);
}


Array<u8> Win32ReadAsset(const char* name, Memory& memory)
{
	char path[MAX_PATH + 64];  // @Unstable(ted): 'MAX_PATH' might be too small.
	char data[] = "../data/";

	for (size_t i = 0; i < sizeof(data)-1; ++i)
		path[i] = data[i];

	for (size_t i = 0; i < MAX_PATH; i++)
		path[sizeof(data) + i] = name[i];


	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
	// "The name [...]. You may use either forward slashes (/) or backslashes () in this name."
	// NOTE(ted): We always user forward slashes.
	HANDLE handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD bytes_read = 0;

	DWORD error = GetLastError();
	if (error == ERROR_FILE_NOT_FOUND)
	{
		WIN32_REPORT_ERROR("Asset '%s' doesn't exist\n", name);
	}
	
	DWORD file_size = GetFileSize(handle, NULL);
	if (file_size == INVALID_FILE_SIZE)
	{
		WIN32_REPORT_ERROR("Invalid size of asset '%s'\n", name);
		// Deallocate asset!
		Array<u8> fail = { 0 };
		return fail;
	}

	Array<u8> array = AllocateArray(memory.persistent, file_size);  // @LEAK(ted): Should not be stored in persistant memory.
	bool success = true;
	if (!ReadFile(handle, array.data, array.count, &bytes_read, NULL))
	{
		WIN32_REPORT_ERROR("Failed to write to '%s'\n", name);
		success = false;
	}
	else if (!CloseHandle(handle))
	{
		WIN32_REPORT_ERROR("Failed to close resource '%s'\n", name);
		success = false;
	}
	else if (bytes_read != array.count)
	{
		WIN32_REPORT_ERROR("Only %d bytes out of %d bytes were read\n", bytes_read, array.count);
		success = false;
	}
	
	if (!success)
	{
		// Deallocate asset!
		Array<u8> fail = { 0 };
		return fail;
	}

	return array;
}


bool Win32WriteAsset(const char* name, Buffer buffer)
{
	// For now, store this directly in a file. In the future, it might be cached in our memory struct as well, and pushed
	// to non-volatile memory if we need more volatile space.

	char path[MAX_PATH + 64];  // @Unstable(ted): 'MAX_PATH' might be too small.
	char data[] = "../data/";

	for (size_t i = 0; i < sizeof(data) - 1; ++i)
		path[i] = data[i];

	for (size_t i = 0; i < MAX_PATH; i++)
		path[sizeof(data) + i] = name[i];

	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
	// "The name [...]. You may use either forward slashes (/) or backslashes () in this name."
	// NOTE(ted): We always user forward slashes.
	HANDLE handle = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return false;

	DWORD error = GetLastError();
	if (error == ERROR_ALREADY_EXISTS)
		WIN32_REPORT_ERROR("Overriding asset '%s'\n", name);  // TODO(ted): Log instead.

	DWORD bytes_written = 0;
	if (!WriteFile(handle, buffer.data, buffer.size, &bytes_written, NULL))
	{
		WIN32_REPORT_ERROR("Failed to write to '%s'\n", name);
		return false;
	}
	else if (!CloseHandle(handle))
	{
		WIN32_REPORT_ERROR("Failed to close resource '%s'\n", name);
		return false;
	}
	else if (bytes_written != buffer.size)
	{
		WIN32_REPORT_ERROR("Only %d bytes out of %d bytes were written\n", bytes_written, buffer.size);
		return false;
	}
	else
	{
		return true;
	}
}

struct FrameStats
{
	f32 time_min,   time_one_fourth,   time_average,   time_three_fourth,   time_max;
	f32 cycles_min, cycles_one_fourth, cycles_average, cycles_three_fourth, cycles_max;
};

FrameStats GetFrameStats(u64* time_results, u8 time_result_count, u64* cycle_results, u8 cycle_result_count)
{
	BubbleSort(time_results,  time_result_count);
	BubbleSort(cycle_results, cycle_result_count);

	u64 i = time_result_count  - 1;
	u64 j = cycle_result_count - 1;

	FrameStats stats =
	{
		time_results[0] / 1000000.0f,  time_results[i / 4] / 1000000.0f,  time_results[i / 2] / 1000000.0f,  time_results[3 * i / 4] / 1000000.0f,  time_results[i] / 1000000.0f,
		cycle_results[0] / 1000000.0f, cycle_results[j / 4] / 1000000.0f, cycle_results[j / 2] / 1000000.0f, cycle_results[3 * j / 4] / 1000000.0f, cycle_results[j] / 1000000.0f
	};

	return stats;
}


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE _, PWSTR command_line_arguments, int show_code)
{
	// Register the window class.
	const char CLASS_NAME[] = "Windows platform";

	WNDCLASS window_class = { 0 };

	window_class.lpfnWndProc = Win32EventCallback;
	window_class.hInstance = instance;
	window_class.lpszClassName = CLASS_NAME;
	window_class.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&window_class))
	{
		return -1;
	}

	// Create the window.
	HWND window = CreateWindowExA(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"Windows platform",				// Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		instance,   // Instance handle
		NULL        // Additional application data
	);

	if (!window)
	{
		return -1;
	}

	ShowWindow(window, show_code);

	InitializeTimeModule();

	static Keyboard keyboard;

	LPVOID start_up_location = (LPVOID)TERABYTES(2);
	SIZE_T memory_size = MEGABYTES(2);
	u8*	   raw_memory  = cast(VirtualAlloc(start_up_location, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE), u8*);

	Memory memory = { 0 };
	memory.persistent.size = memory_size / 2;
	memory.persistent.used = 0;
	memory.persistent.data = raw_memory;
	memory.temporary.size  = memory_size / 2;
	memory.temporary.used  = 0;
	memory.temporary.data  = raw_memory + memory_size / 2;
	memory.initialized = false;
	memory.WriteAsset = Win32WriteAsset;
	memory.ReadAsset  = Win32ReadAsset;


	char path[MAX_PATH];  // NOTE(ted): 'MAX_PATH' can be too small...
	Win32GetExecutableDirectory(path, sizeof(path));

	char source[MAX_PATH + sizeof("main.dll")];
	char destination[MAX_PATH + sizeof("main_temp.dll")];

	{
		int i;
		for (i = 0; i < MAX_PATH; ++i)
		{
			if (path[i] == '\0')
				break;
			source[i] = path[i];
			destination[i] = path[i];
		}
		int j = i;
		for (char character : "main.dll")
		{
			source[i++] = character;
		}
		for (char character : "main_temp.dll")
		{
			destination[j++] = character;
		}
		source[i++] = '\0';
		destination[j++] = '\0';
	}

	
	Win32Game win32_game = { 0 };
	win32_game.source_dll  = source;
	win32_game.runtime_dll = destination;
	Win32LoadGame(win32_game);

	win32_game.initialize(memory);

	// @TODO(ted): Query the actual refresh rate.
	u32 monitor_refresh_rate = 60;
	u32 game_refresh_rate = monitor_refresh_rate / 2;  // @NOTE(ted): The game should have a fixed update rate that is a multiple of the monitor's refresh rate.
	f32 target_seconds_per_frame = 1.0f / game_refresh_rate;

	NanoClock frame_clock;
	NanoClock game_clock;
	NanoClock stat_output_clock;

	// Stats
	u8  frame_time_result_count = 0;
	u64 frame_time_results[255];
	u8  frame_cycle_result_count = 0;
	u64 frame_cycle_results[255];

	u8  game_time_result_count = 0;
	u64 game_time_results[255];
	u8  game_cycle_result_count = 0;
	u64 game_cycle_results[255];

	u16 frames = 0;

	running = true;
	while (running)
	{
		u64 frame_cylce_start = CycleCount();

		Win32ProcessEvents(keyboard);

		Tick(game_clock);
		u64 game_cylce_start = CycleCount();
		win32_game.update(memory, game_framebuffer, keyboard, target_seconds_per_frame);
		game_cycle_results[game_cycle_result_count++] = CycleCount() - game_cylce_start;
		game_time_results[game_time_result_count++]   = Tick(game_clock);;


		WIN32_ASSERT(win32_framebuffer.width == game_framebuffer.width && win32_framebuffer.height == game_framebuffer.height, "Win32 framebuffer and game framebuffer is not synchronized!");
		for (int i = 0; i < game_framebuffer.width * game_framebuffer.height; ++i)
		{
			Pixel& color = game_framebuffer.pixels[i];
			win32_framebuffer.pixels[i] = PackBGRA(
				ExactLinearTosRGB(color.r) * 255.0f,
				ExactLinearTosRGB(color.g) * 255.0f,
				ExactLinearTosRGB(color.b) * 255.0f,
				ExactLinearTosRGB(color.a) * 255.0f
			);
		}

		RECT client_rect;
		GetClientRect(window, &client_rect);

		HDC device_context = GetDC(window);
		Win32UpdateWindow(device_context, win32_framebuffer, client_rect);
		ReleaseDC(window, device_context);

		Win32HotloadGame(win32_game, source, destination);


		u64 dt = Tick(frame_clock, RoundToU32(SECONDS_TO_NANO(target_seconds_per_frame)));

		// ---- FRAME COUNT ----
		frame_cycle_results[frame_cycle_result_count++] = CycleCount() - frame_cylce_start;
		frame_time_results[frame_time_result_count++] = dt;
		++frames;

		if (Timer(stat_output_clock, SECONDS_TO_NANO(1)))
		{
			static char buffer[1024] = { 0 };

			FrameStats frame_stats = GetFrameStats(frame_time_results, frame_time_result_count, frame_cycle_results, frame_cycle_result_count);
			FrameStats game_stats  = GetFrameStats(game_time_results,  game_time_result_count,  game_cycle_results,  game_cycle_result_count);
			
			sprintf(buffer,
				"\n	-------- STATS --------\n"
				"\t                    :  MIN   | 1/4'th |  AVG   | 3/4'th |  MAX \n"
				"\tFrame: Time (ms)    : %6.2f | %6.2f | %6.2f | %6.2f | %6.2f\n"
				"\tFrame: Cycles (MHz) : %6.2f | %6.2f | %6.2f | %6.2f | %6.2f\n"
				"\tGame:  Time (ms)    : %6.2f | %6.2f | %6.2f | %6.2f | %6.2f\n"
				"\tGame:  Cycles (MHz) : %6.2f | %6.2f | %6.2f | %6.2f | %6.2f\n"
				"\tFrames per second   :  %i\n",
				frame_stats.time_min,   frame_stats.time_one_fourth,   frame_stats.time_average,   frame_stats.time_three_fourth,   frame_stats.time_max,
				frame_stats.cycles_min, frame_stats.cycles_one_fourth, frame_stats.cycles_average, frame_stats.cycles_three_fourth, frame_stats.cycles_max,
				game_stats.time_min,    game_stats.time_one_fourth,    game_stats.time_average,    game_stats.time_three_fourth,    game_stats.time_max,
				game_stats.cycles_min,  game_stats.cycles_one_fourth,  game_stats.cycles_average,  game_stats.cycles_three_fourth,  game_stats.cycles_max,
				frames
			);
			OutputDebugStringA(buffer);

			frames = 0;
			frame_cycle_result_count = 0;
			frame_time_result_count  = 0;
			game_cycle_result_count  = 0;
			game_time_result_count   = 0;
		}
	}

	return 0;
}