#include "main.h"

#include <windows.h>
#include <xinput.h>
#include <dsound.h>


#define WIN32_REPORT_ERROR(...)                                                                         \
do {                                                                                                    \
    char buffer1[255] = { 0 };                                                                          \
	char buffer2[255] = { 0 };                                                                          \
    sprintf(buffer1, __VA_ARGS__);                                                                      \
	sprintf(buffer2, "[Error]:\n\tFile: %s\n\tLine: %i\n\tMessage: %s", __FILE__, __LINE__, buffer1);   \
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
	wchar_t  source[MAX_PATH];

	InitializeFunction initialize;
	UpdateFunction     update;
	SoundFunction      sound;
};

struct Win32PerformanceCounter
{
	LARGE_INTEGER performance_counter;
	u64 cycles_counter;
};

static Win32FrameBuffer win32_framebuffer;
static FrameBuffer game_framebuffer;
static bool running;

static LARGE_INTEGER performance_frequency;


static Win32PerformanceCounter Win32CreatePerformanceCounter()
{
	Win32PerformanceCounter counter = { 0 };

	QueryPerformanceCounter(&counter.performance_counter);
	counter.cycles_counter = __rdtsc();

	return counter;
}


static f32 Win32TickPerformanceCounter(Win32PerformanceCounter& start)
{
	Win32PerformanceCounter stop = Win32CreatePerformanceCounter();

	f32 performance_counter = 1000.0f * cast(stop.performance_counter.QuadPart - start.performance_counter.QuadPart, f32) / performance_frequency.QuadPart;
	f32 cycles_counter = cast(stop.cycles_counter - start.cycles_counter, f32) / (1000.0f * 1000.0f);
	f32 fps_counter = cast(performance_frequency.QuadPart, f32) / (stop.performance_counter.QuadPart - start.performance_counter.QuadPart);

	static char buffer[255] = { 0 };
	sprintf(buffer, "Milliseconds per frame: %.02f  |  FPS: %0.2f  |  Cycles per frame: %0.2f * 10^6\n", performance_counter, fps_counter, cycles_counter);
	//OutputDebugStringA(buffer);

	start.performance_counter = stop.performance_counter;
	start.cycles_counter = stop.cycles_counter;

	f32 dt = performance_counter / 1000.0f;

	return dt;
}


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
	int win32_buffer_memory_size = win32_buffer.width * win32_buffer.height * win32_buffer.bytes_per_pixel;
	win32_buffer.pixels = cast(VirtualAlloc(0, win32_buffer_memory_size, MEM_COMMIT, PAGE_READWRITE), u32*);

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


static void Win32GetExecutableDirectory(TCHAR* path, size_t size)
{
	DWORD length = GetModuleFileNameW(NULL, path, size);

	for (int i = length; i >= 0; --i)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = '\0';
			return;
		}
	}
	
	path[0] = '\0';
}


static FILETIME Win32GetLastFileWriteTime(const wchar_t* filename)
{
	FILETIME write_time = { 0 };

	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(filename, &data);

	if (handle != INVALID_HANDLE_VALUE)
	{
		write_time = data.ftLastWriteTime;
		FindClose(handle);
	}

	return write_time;
}


static void Win32LoadGame(Win32Game& game, const wchar_t* source, wchar_t* destination)
{
	// Open the DLL to see if it's locked or not. If it is, try again later.
	HANDLE dll_file = CreateFile(source, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (dll_file == INVALID_HANDLE_VALUE)
		return;

	// Unload the currently loaded DLL.
	if (game.dll_handle)
		WIN32_ASSERT(FreeLibrary(game.dll_handle) != 0, "Unable to free dll library.\n");

	// Copy the file so the 'source' file is still available for editing whem game is running.
	WIN32_ASSERT(CopyFile(source, destination, FALSE) != 0, "Copy failed.\n");

	// Load the copy DLL.
	game.dll_handle = LoadLibrary(destination);
	
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

static void Win32HotloadGame(Win32Game& game, const wchar_t* source, wchar_t* destination)
{
	FILETIME write_time = Win32GetLastFileWriteTime(source);
	if (CompareFileTime(&write_time, &game.time_loaded) == 0)
		return;
	else
		Win32LoadGame(game, source, destination);

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

	QueryPerformanceFrequency(&performance_frequency);

	static Keyboard keyboard;

	LPVOID start_up_location = (LPVOID)TERABYTES(2);
	SIZE_T memory_size = MEGABYTES(2);
	u8*	   raw_memory  = cast(VirtualAlloc(start_up_location, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE), u8*);

	Memory memory = { 0 };
	memory.persistent.count = memory_size / 2;
	memory.persistent.used  = 0;
	memory.persistent.data  = raw_memory;
	memory.temporary.count  = memory_size / 2;
	memory.temporary.used	= 0;
	memory.temporary.data	= raw_memory + memory_size / 2;
	memory.initialized = false;


	wchar_t path[MAX_PATH];  // NOTE(ted): 'MAX_PATH' can be too small...
	Win32GetExecutableDirectory(path, sizeof(path));

	wchar_t source[MAX_PATH + sizeof(L"main.dll")];
	wchar_t destination[MAX_PATH + sizeof(L"main_temp.dll")];
	
	int i;
	for (i = 0; i < MAX_PATH; ++i)
	{
		if (path[i] == '\0')
			break;
		source[i] = path[i];
		destination[i] = path[i];
	}
	int j = i;
	for (wchar_t character : L"main.dll")
	{
		source[i++] = character;
	}
	for (wchar_t character : L"main_temp.dll")
	{
		destination[j++] = character;
	}
	source[i++] = '\0';
	destination[j++] = '\0';



	Win32Game win32_game = { 0 };
	Win32LoadGame(win32_game, source, destination);
	win32_game.initialize(memory);

	Win32PerformanceCounter start = Win32CreatePerformanceCounter();

	running = true;
	while (running)
	{
		f32 dt = Win32TickPerformanceCounter(start);

		Win32ProcessEvents(keyboard);

		win32_game.update(memory, game_framebuffer, keyboard, dt);


		RECT client_rect;
		GetClientRect(window, &client_rect);

		u32 width  = client_rect.right  - client_rect.left;
		u32 height = client_rect.bottom - client_rect.top;

		HDC device_context = GetDC(window);
		Win32UpdateWindow(device_context, win32_framebuffer, game_framebuffer, width, height);

		
		Win32HotloadGame(win32_game, source, destination);
	}

	return 0;
}