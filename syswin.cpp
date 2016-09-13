#include <Windows.h>
#include <stdio.h>

typedef char		i8;
typedef short		i16;
typedef int			i32;
typedef long long	i64;

typedef i32			b32;
typedef float		f32;
typedef double		f64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef size_t mem_size;

#include "engine.h"

b32 isRunning = false;
LARGE_INTEGER globalPerfFreq;

struct Win32Buffer
{
	BITMAPINFO bmi;
	i32 width;
	i32 height;
	u32 bpp;
	u32 pitch;
	void* data;
};

struct Win32WndDim
{
	i32 width;
	i32 height;
};

Win32Buffer globalBuffer;

Win32WndDim Win32GetWindowDims(HWND window)
{
	Win32WndDim Result = {};
	RECT rect = {};
	GetClientRect(window, &rect);

	Result.width = rect.right - rect.left;
	Result.height = rect.bottom - rect.top;
	return Result;
}

struct FileResult
{
	void* data;
	i64 fileSize;
};

void Win32FreeMemory(void* memory)
{
	if (memory)
	{
		VirtualFree(memory, 0, MEM_RELEASE);
		memory = 0;
	}
}

#define PLATFORM_READ_ENTIRE_FILE(name) FileResult name(i8* filename)
PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile)
{
	FileResult Result = {};
	HANDLE file = CreateFile(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

	if (file != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER fileSize = {};
		GetFileSizeEx(file, &fileSize);
		i64 size = fileSize.QuadPart;

		Result.data = VirtualAlloc(0, (mem_size)size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (Result.data)
		{
			DWORD bytesRead = 0;
			if (ReadFile(file, Result.data, (DWORD)size, &bytesRead, 0) && size == bytesRead)
			{
				Result.fileSize = bytesRead;
			}
		}

		CloseHandle(file);
	}

	return Result;
}

#define PLATFORM_WRITE_ENTIRE_FILE(name) void name(i8* filename, FileResult fileData)
PLATFORM_WRITE_ENTIRE_FILE(PlatformWriteEntireFile)
{
	HANDLE file = CreateFile(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWritten = 0;
		WriteFile(file, fileData.data, (DWORD)fileData.fileSize, &bytesWritten, 0);
	}
	CloseHandle(file);
}

void Win32ResizeDIBSection(Win32Buffer* Buffer, i32 width, i32 height)
{
	if (Buffer->data)
	{
		VirtualFree(Buffer->data, 0, MEM_RELEASE);
	}

	Buffer->width = width;
	Buffer->height = height;
	Buffer->bpp = 4;
	Buffer->pitch = Buffer->bpp * Buffer->width;

	Buffer->bmi.bmiHeader.biSize = sizeof(Buffer->bmi.bmiHeader);
	Buffer->bmi.bmiHeader.biWidth = Buffer->width;
	Buffer->bmi.bmiHeader.biHeight = -Buffer->height;
	Buffer->bmi.bmiHeader.biPlanes = 1;
	Buffer->bmi.bmiHeader.biBitCount = 32;
	Buffer->bmi.bmiHeader.biCompression = BI_RGB;
	
	i32 BufferSize = Buffer->bpp * (Buffer->width * Buffer->height);

	Buffer->data = VirtualAlloc(0, BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void Win32UpdateWindow(HDC device, Win32Buffer* Buffer, i32 width, i32 height)
{
	StretchDIBits(device, 
		0, 0, Buffer->width, Buffer->height,
		0, 0, Buffer->width, Buffer->height, 
		Buffer->data, &Buffer->bmi, 
		DIB_RGB_COLORS, SRCCOPY);
}

i64 Win32GetClock()
{
	LARGE_INTEGER Result = {};
	QueryPerformanceCounter(&Result);
	return Result.QuadPart;
}

f64 Win32GetSecondsElapsed(i64 start, i64 end)
{
	f64 Result = (f64)(end - start) / (f64)globalPerfFreq.QuadPart;
	return Result;
}

LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT Result = 0;

	switch (msg)
	{
	case WM_CREATE:
	{

	}break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC device = BeginPaint(window, &ps);

		Win32WndDim WndDim = Win32GetWindowDims(window);
		Win32UpdateWindow(device, &globalBuffer, WndDim.width, WndDim.height);

		EndPaint(window, &ps);
	}break;

	case WM_CLOSE:
	{
		isRunning = false;
	}break;

	default:
	{
		Result = DefWindowProc(window, msg, wparam, lparam);
	}break;
	}

	return Result;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow)
{
	WNDCLASS wc = {0};
	
	Win32ResizeDIBSection(&globalBuffer, 800, 600);

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hInstance = hInst;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.lpszClassName = "WulfEngine";
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClass(&wc);

	HWND window = CreateWindow("WulfEngine", "WulfEngine",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, CW_USEDEFAULT,
								1024, 768,
								0, 0, hInst, 0);

	if (!window)
		return -1;

	UpdateWindow(window);
	ShowWindow(window, SW_SHOW);

	GameMemory memory = {};
	memory.permanentSize = Megabytes(64);
	memory.transientSize = Megabytes(64);
	mem_size totalSize = memory.permanentSize + memory.transientSize;
	void* memoryStorage = VirtualAlloc(0, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	memory.permanentStorage = memoryStorage;
	memory.transientStorage = ((u8*)memoryStorage + memory.permanentSize);

	if (memoryStorage)
	{
		isRunning = true;

		QueryPerformanceFrequency(&globalPerfFreq);

		i32 desiredRefreshRate = GetDeviceCaps(GetDC(window), VREFRESH);
		i32 monitorHz = 0;

		if (desiredRefreshRate <= 0)
		{
			monitorHz = 1;
		}
		else
		{
			monitorHz = desiredRefreshRate;
		}
		f32 dt = 1.0f / monitorHz;

		i64 startCounter = Win32GetClock();

		i64 startCycles = __rdtsc();

		GameController input = {};

		while (isRunning)
		{
			MSG msg = {};
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				switch (msg.message)
				{
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				case WM_KEYDOWN:
				case WM_KEYUP:
				{
					u32 vkCode = (u32)msg.wParam;
					b32 isDown = ((1 << 30) & msg.lParam) == 0;
					b32 wasDown = ((1 << 31) & msg.lParam) != 0;

					if (isDown != wasDown)
					{
						if (vkCode == VK_ESCAPE)
						{
							input.back.isDown = isDown;
						}
						else if (vkCode == VK_SPACE)
						{
							input.start.isDown = isDown;
						}
						else if (vkCode == 'W')
						{
							input.moveUp.isDown = isDown;
						}
						else if (vkCode == 'S')
						{
							input.moveDown.isDown = isDown;
						}
						else if (vkCode == 'A')
						{
							input.moveLeft.isDown = isDown;
						}
						else if (vkCode == 'D')
						{
							input.moveRight.isDown = isDown;
						}
					}
				}break;

				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}break;
				}
			}

			if (input.back.isDown)
			{
				isRunning = false;
				PostQuitMessage(0);
			}

			Buffer buffer = {};
			buffer.data = globalBuffer.data;
			buffer.width = globalBuffer.width;
			buffer.height = globalBuffer.height;
			buffer.bpp = globalBuffer.bpp;
			buffer.pitch = globalBuffer.pitch;

			GameUpdateAndRender(dt, &buffer, &memory, &input);

			HDC device = GetDC(window);
			Win32WndDim WndDim = Win32GetWindowDims(window);
			Win32UpdateWindow(device, &globalBuffer, WndDim.width, WndDim.height);

			i64 endCycles = __rdtsc();

			i64 endCounter = Win32GetClock();

			i64 elapsedCycles = endCycles - startCycles;
			i64 elapsedCounter = (endCounter - startCounter);

			f64 MSPF = (Win32GetSecondsElapsed(startCounter, endCounter)) * 1000.0f;
			f64 FPS = (f64)(globalPerfFreq.QuadPart / elapsedCounter);
			f64 MCPF = (f64)(elapsedCycles) / (1000.0f * 1000.0f);

			char perfBuffer[256];
			sprintf(perfBuffer, "%.02fmspf %.02ffps %.02fmcpf\n", MSPF, FPS, MCPF);
			OutputDebugString(perfBuffer);

			startCounter = endCounter;
			startCycles = endCycles;
		}

		Win32FreeMemory(memoryStorage);
	}

	UnregisterClass("WulfEngine", hInst);
	DestroyWindow(window);
	Win32FreeMemory(globalBuffer.data);

	return 0;
}