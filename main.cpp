#include <windows.h>
//#include <gl/glew.h>
#include <gl/gl.h>
//#include <gl/glu.h>
#include <iostream>
#include <thread>
#include "fourier.h"

using namespace std;

DFT dft;
size_t N;
double* nums = nullptr;
double* reconstructed = nullptr;
double reconstruct_samples;
Complex* result = nullptr;

HINSTANCE hInstance;
HWND hWnd;
HDC hDC;
HGLRC hRC;
const char* window_name = "Signal";
int width = 1280;
int height = 720;
int MonitorX;
int MonitorY;
int MouseX;
int MouseY;
bool fullscreen = false;
bool keys[256];
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void CreateOGLWindow(int, int, bool);
void GetMonitorParameters(int&, int&);
void SetVSync(bool);
void Initialize();
void DrawGL();
int main(int argc, char** argv)
{
	WNDCLASS wc;
	MSG msg;
	BOOL bQuit = FALSE;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = window_name;
	RegisterClass(&wc);
	GetMonitorParameters(MonitorX, MonitorY);
	CreateOGLWindow(width, height, fullscreen);
	EnableOpenGL(hWnd, &hDC, &hRC);

	DrawGL();
	SwapBuffers(hDC);
	while (!bQuit)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bQuit = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (!bQuit)
			{
				/*
				DrawGL();
				SwapBuffers(hDC);
				*/
			}
			if (keys[VK_ESCAPE])
			{
				bQuit = true;
			}
		}
	}
	DisableOpenGL(hWnd, hDC, hRC);
	DestroyWindow(hWnd);
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_DESTROY:
		return 0;
	case WM_KEYDOWN:
		keys[wParam] = true;
		return 0;
	case WM_KEYUP:
		keys[wParam] = false;
		return 0;
	case WM_MOUSEMOVE:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
		return 0;
	case WM_LBUTTONDOWN:
		return 0;
	case WM_LBUTTONUP:
		return 0;
	case WM_MOUSEWHEEL:
		switch (HIWORD(wParam))
		{
		case 120: // Scroll up
			break;
		case 65416: // Scroll down
			break;
		}
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	*hDC = GetDC(hWnd);
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ChoosePixelFormat(*hDC, &pfd);
	SetPixelFormat(*hDC, iFormat, &pfd);
	*hRC = wglCreateContext(*hDC);
	wglMakeCurrent(*hDC, *hRC);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.f, (float)width / (float)height, 0.1, 10000);
	glOrtho(-width / 2, width / 2, -height / 2, height / 2, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLineWidth(2);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	Initialize();
	SetVSync(false);
}
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	delete[] nums;
	delete[] reconstructed;
	delete[] result;
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}
void CreateOGLWindow(int width, int height, bool full)
{
	RECT Client_Rect;
	DWORD style;
	Client_Rect.left = (MonitorX - width) / 2;
	Client_Rect.right = Client_Rect.left + width;
	Client_Rect.top = (MonitorY - height) / 2;
	Client_Rect.bottom = Client_Rect.top + height;
	if (full)
	{
		style = WS_POPUP | WS_VISIBLE;
		AdjustWindowRect(&Client_Rect, style, false);
		hWnd = CreateWindow(
			window_name, window_name,
			style,
			0, 0, MonitorX, MonitorY,
			NULL, NULL, hInstance, NULL);
	}
	else
	{
		style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
		AdjustWindowRect(&Client_Rect, style, false);
		hWnd = CreateWindow(
			window_name, window_name,
			style,
			Client_Rect.left, Client_Rect.top, Client_Rect.right - Client_Rect.left, Client_Rect.bottom - Client_Rect.top,
			NULL, NULL, hInstance, NULL);
	}
}
void GetMonitorParameters(int &x, int &y)
{
	HWND desktop;
	RECT DESKRECT;
	desktop = GetDesktopWindow();
	GetWindowRect(desktop, &DESKRECT);
	x = DESKRECT.right;
	y = DESKRECT.bottom;
	CloseHandle(desktop);
}
void SetVSync(bool sync)
{
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
	const char *extensions = (char*)glGetString(GL_EXTENSIONS);
	if (strstr(extensions, "WGL_EXT_swap_control"))
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT(sync);
		}
	}
}
void Initialize()
{
	N = 256;
	reconstruct_samples = 1;
	dft.Initialize(N);
	nums = new double[N]();
	reconstructed = new double[reconstruct_samples*N]();
	size_t half = N / 2;
	double* amp = new double[half];
	double* phase = new double[half];
	double t = 0;
	for (size_t i = 0; i < N; ++i, t += 0.6) // Input signal
	{
		//nums[i] = sin(30 * (t / rad)) * 100 + sin(17 * t / rad) * 60 + cos(25 * t / rad) * 75;
		//nums[i] = sqrt(t) * 20;// +sin(34 * t / rad) * 30;
		nums[i] = 100 * ((i * 8 / N) % 2);
	}
	result = dft.Transform(nums);
	for (size_t i = 0; i < half; ++i)
	{
		amp[i] = result[i].mod() * 2 / (double)N;
		phase[i] = result[i].arg();
	}
	for (size_t i = 0; i < reconstruct_samples*N; ++i)
	{
		reconstructed[i] += amp[0] * 0.5;
		for (size_t j = 1; j < half; ++j)
		{
			reconstructed[i] += amp[j] * cos((double)i/ reconstruct_samples * j * 2 * pi / N + phase[j]);
		}
	}
	delete[] amp;
	delete[] phase;
}
void DrawGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	double x = -(double)N / 2.0;
	double scale = (double)width / (double)N;
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < N; ++i, x += 1)
	{
		glVertex2d(x * scale, nums[i]);
	}
	glEnd();
	x = -(double)N / 2.0;
	glColor3f(0, 0.6, 1);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < N / 2; ++i, x += 1)
	{
		glVertex2d(x * scale * 2 + width / 2, result[i].mod() / 100.0);
	}
	glEnd();
	x = -(double)N / 2.0;
	glColor3f(1, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < reconstruct_samples*N; ++i, x += 1.0 / reconstruct_samples)
	{
		glVertex2d(x * scale, reconstructed[i]);
	}
	glEnd();
}
