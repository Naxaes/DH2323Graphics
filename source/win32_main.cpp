#include "main.h"

#include <windows.h>
#include <xinput.h>
#include <dsound.h>


// struct Clock;
// float Tick(Clock& clock);

#define WIN32_REPORT_ERROR(...)                                                                               \
{                                                                                                       \
    char buffer1[255] = { 0 };                                                                          \
	char buffer2[255] = { 0 };                                                                          \
    sprintf(buffer1, __VA_ARGS__);                                                                      \
	sprintf(buffer2, "[Error]:\n\tFile: %s\n\tLine: %i\n\tMessage: %s", __FILE__, __LINE__, buffer1);   \
    OutputDebugStringA(buffer2);																		\
}
#define WIN32_ASSERT(condition, ...)			\
{												\
	if (!(condition))							\
	{											\
		WIN32_REPORT_ERROR(__VA_ARGS__);		\
		exit(-1);								\
	}											\
}



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
	InitializeFunction initialize;
	UpdateFunction     update;
	SoundFunction      sound;
};

static Win32FrameBuffer win32_framebuffer;
static FrameBuffer game_framebuffer;
static Win32Game win32_game;
static bool running;


// TODO(ted): Pre-allocate the maximum size at startup. The user should always be able
// to resize to the maximum size, so if we pre-allocate we know that the game won't
// crash at resize due to memory. It'll also simplify the code and make it more efficient.
static void Win32ResizeFrameBuffer(Win32FrameBuffer& win32_buffer, FrameBuffer& framebuffer, int width, int height)
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
	int memory_size = win32_buffer.width * win32_buffer.height * win32_buffer.bytes_per_pixel;
	win32_buffer.pixels = cast(VirtualAlloc(0, memory_size, MEM_COMMIT, PAGE_READWRITE), u32*);

	framebuffer.width  = width;
	framebuffer.height = height;
	int framebuffer_memory_size = framebuffer.width * framebuffer.height * sizeof(Pixel);
	framebuffer.pixels = cast(VirtualAlloc(0, framebuffer_memory_size, MEM_COMMIT, PAGE_READWRITE), Pixel*);
}


static void Win32UpdateWindow(HDC device_context, Win32FrameBuffer& win32_buffer, FrameBuffer& framebuffer, u32 width, u32 height)
{
	WIN32_ASSERT(win32_buffer.width == framebuffer.width && win32_buffer.height == framebuffer.height, "Win32 buffer and Framebuffer is asynchronized!");

	for (int i = 0; i < framebuffer.width * framebuffer.height; ++i)
	{
		Pixel& color = framebuffer.pixels[i];
		win32_buffer.pixels[i] = PackBGRA(
			ExactLinearTosRGB(color.r) * 255.0f,
			ExactLinearTosRGB(color.g) * 255.0f,
			ExactLinearTosRGB(color.b) * 255.0f,
			ExactLinearTosRGB(color.a) * 255.0f
		);
	}

	int status = StretchDIBits(
		device_context,
		0, 0, width, height,   // Destination
		0, 0, width, height,   // Source
		win32_buffer.pixels, &win32_buffer.info,
		DIB_RGB_COLORS, SRCCOPY
	);

	WIN32_ASSERT(status != 0, "Something is wrong!!!");
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
			RECT client_rect;
			GetClientRect(window, &client_rect);

			u32 width  = client_rect.right  - client_rect.left;
			u32 height = client_rect.bottom - client_rect.top;

			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			
			Win32UpdateWindow(device_context, win32_framebuffer, game_framebuffer, width, height);
	
			EndPaint(window, &paint);
		} break;
		case WM_SIZE:
		{
			RECT client_rect;
			GetClientRect(window, &client_rect);
			int width  = client_rect.right  - client_rect.left;
			int height = client_rect.bottom - client_rect.top;

			Win32ResizeFrameBuffer(win32_framebuffer, game_framebuffer, width, height);
		} break;
		case WM_CLOSE:
		{
			if (MessageBox(window, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
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


static void Win32LoadGame(Win32Game& game)
{
	HMODULE dll_handle = LoadLibrary(L"main.dll");
	if (!dll_handle)
	{
		WIN32_REPORT_ERROR("Couldn't load game!\n")
	}

	game.initialize = reinterpret_cast<InitializeFunction>(GetProcAddress(dll_handle, "Initialize"));
	if (!game.initialize)
		game.initialize = DEFAULT_Initialize;

	game.update = reinterpret_cast<UpdateFunction>(GetProcAddress(dll_handle, "Update"));
	if (!game.update)
		game.update = DEFAULT_Update;

	game.sound = reinterpret_cast<SoundFunction>(GetProcAddress(dll_handle, "Sound"));
	if (!game.sound)
		game.sound = DEFAULT_Sound;

	if (!game.initialize || !game.update || !game.sound)
	{
		WIN32_REPORT_ERROR("Couldn't load all game functions!\n")
	}
}


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE _, PWSTR command_line_arguments, int show_code)
{
	// Register the window class.
	const WCHAR CLASS_NAME[] = L"Windows platform";

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
	HWND window = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Windows platform",			// Window text
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

	static Keyboard keyboard;

	LARGE_INTEGER performance_frequency;
	QueryPerformanceFrequency(&performance_frequency);

	LPVOID start_up_location = (LPVOID)TERABYTES(2);
	SIZE_T memory_size = MEGABYTES(2);
	u8* raw_memory = cast(VirtualAlloc(start_up_location, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE), u8*);

	Memory memory = { 0 };
	memory.persistent.count = memory_size / 2;
	memory.persistent.used  = 0;
	memory.persistent.data  = raw_memory;
	memory.temporary.count  = memory_size / 2;
	memory.temporary.used	= 0;
	memory.temporary.data	= raw_memory + memory_size / 2;
	memory.initialized = false;

	Win32LoadGame(win32_game);

	win32_game.initialize(memory);

	LARGE_INTEGER performance_counter_start;
	QueryPerformanceCounter(&performance_counter_start);
	u64 cycles_counter_start = __rdtsc();

	running = true;
	while (running)
	{
		LARGE_INTEGER performance_counter_stop;
		QueryPerformanceCounter(&performance_counter_stop);

		u64 cycles_counter_stop = __rdtsc();

		f32 performance_counter = 1000.0f * cast(performance_counter_stop.QuadPart - performance_counter_start.QuadPart, f32) / performance_frequency.QuadPart;
		f32 cycles_counter = cast(cycles_counter_stop - cycles_counter_start, f32) / (1000.0f * 1000.0f);
		f32 fps_counter = cast(performance_frequency.QuadPart, f32) / (performance_counter_stop.QuadPart - performance_counter_start.QuadPart);

		static char buffer[255] = { 0 };
		sprintf(buffer, "Milliseconds per frame: %.02f  |  FPS: %0.2f  |  Cycles per frame: %0.2f * 10^6\n", performance_counter, fps_counter, cycles_counter);
		OutputDebugStringA(buffer);

		performance_counter_start = performance_counter_stop;
		cycles_counter_start = cycles_counter_stop;

		f32 dt = performance_counter / 1000.0f;



		Win32ProcessEvents(keyboard);

		win32_game.update(memory, game_framebuffer, keyboard, dt);


		RECT client_rect;
		GetClientRect(window, &client_rect);

		u32 width  = client_rect.right  - client_rect.left;
		u32 height = client_rect.bottom - client_rect.top;

		HDC device_context = GetDC(window);
		Win32UpdateWindow(device_context, win32_framebuffer, game_framebuffer, width, height);

	}

	return 0;
}