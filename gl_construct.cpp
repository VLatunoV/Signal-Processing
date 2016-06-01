#include "gl_construct.h"

extern HINSTANCE hInstance;
extern HWND hWnd;
extern HDC hDC;
extern HGLRC hRC;
extern const char* window_name;
extern int width;
extern int height;
extern int MonitorX;
extern int MonitorY;
extern int MouseX;
extern int MouseY;
extern bool fullscreen;
extern bool keys[];

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
	glOrtho(-width / 2, width / 2, -height / 2, height / 2, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLineWidth(2);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	SetVSync(false);
}
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
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
