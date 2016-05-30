#include <windows.h>
//#include <gl/glew.h>
#include <gl/gl.h>
//#include <gl/glu.h>
#include <iostream>
#include <thread>
#include "fourier.h"
#include "audio_layer.h"
#include "font.h"

using namespace std;

AudioLayer::CaptureEndpoint* MyMicrophone;
DFT dft;
FFT fft;
size_t N;
size_t P;
Cyclic<float> nums;
float* reconstructed = nullptr;
float reconstruct_samples;
float* amp = nullptr;
float* phase = nullptr;
Complex<float>* result = nullptr;
Cyclic<float> input;
BYTE* sound = nullptr;
bool test;
size_t max_freq = 0;

float samplingIndex = 0.0;
float scaling;
float scale_correction;
size_t soundLength;

unsigned int spectrogram;
float tex_x = 1;
unsigned char* update_texture = nullptr;
unsigned char* pallet_red = nullptr;
unsigned char* pallet_green = nullptr;
unsigned char* pallet_blue = nullptr;
const size_t pallet_size = 200;

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

	test = false;
	P = 12;
	N = (size_t)1 << P;

	EnableOpenGL(hWnd, &hDC, &hRC);

	UINT32 timeInterval;
	if (!test)
	{
		Initialize_Font();
		fft.Initialize(P);
		AudioLayer::Initialize();
		MyMicrophone = AudioLayer::GetDefaultCaptureEndpoint();

		//timeInterval = N * 1000 / MyMicrophone->GetWaveFormat()->nSamplesPerSec;
		timeInterval = 20;
		soundLength = timeInterval * MyMicrophone->GetWaveFormat()->nAvgBytesPerSec / 1000;
		scale_correction = 1.0;
		scaling = (float)MyMicrophone->GetWaveFormat()->nSamplesPerSec / (float)N / scale_correction;
		MyMicrophone->Initialize(timeInterval);
	
		MyMicrophone->Start();
		//sound = new BYTE[N * 8]();
		sound = new BYTE[soundLength]();
		//sound = new BYTE[MyMicrophone->GetWaveFormat()->nAvgBytesPerSec]();
		result = new Complex<float>[N];
		amp = new float[N / 2];
		phase = new float[N / 2];
		update_texture = new unsigned char[2 * N];
		pallet_blue = new unsigned char[pallet_size];
		pallet_green = new unsigned char[pallet_size];
		pallet_red = new unsigned char[pallet_size];
		float I;
		for (int a = 0; a < pallet_size; ++a)
		{
			I = (float)a * 255 / (float)pallet_size;
			pallet_red[a] = I;
			pallet_green[a] = 0;
			pallet_blue[a] = I;
		}
		for (int a = 0; a < N / 2; ++a)
		{
			update_texture[a * 4 + 3] = 255;
		}
		//input = new float[N]();
		input.Initialize(P);
	}
	int var1;
	

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
				if(!test)
				{
					MyMicrophone->Capture(timeInterval, sound);
					int i;
					for (i = 0; samplingIndex < soundLength / 8; samplingIndex += scaling, ++i)
					{
						input.push_back(((float*)sound)[int(samplingIndex) * 2]);
						//input.push_back((float)(((float*)sound)[i]));
					}
					//std::cout << "Samples: " << i << "\n";
					samplingIndex -= soundLength / 8;
					if (input.GetSize() == N)
					{
						fft.Transform(input, result);
						max_freq = 0;
						for (UINT32 i = 0; i < N / 2; ++i)
						{
							amp[i] = result[i].mod() * 2 * 1000;
							//amp[i] = result[i].mod_sqr() * N * 100;
							//phase[i] = result[i].arg();
							if (amp[i] > amp[max_freq])
							{
								max_freq = i;
							}
						}
						for (i = 0; i < N / 2; ++i)
						{
							var1 = (amp[i] * pallet_size);
							var1 = var1 > pallet_size - 1 ? pallet_size - 1 : var1;
							update_texture[i * 4] = pallet_red[var1];
							update_texture[i * 4 + 1] = pallet_green[var1];
							update_texture[i * 4 + 2] = pallet_blue[var1];
						}
						glBindTexture(GL_TEXTURE_2D, spectrogram);
						glTexSubImage2D(GL_TEXTURE_2D, 0, (int)(tex_x * width), 0, 1, N / 2, GL_RGBA, GL_UNSIGNED_BYTE, update_texture);
						tex_x += 1.f / width;
						if (tex_x > 1.0)
						{
							tex_x -= 1.0;
						}

					}
					/*for (size_t i = 0; i < N; ++i)
					{
						reconstructed[i] = 0;
						reconstructed[i] += amp[0] * 0.5;
						for (size_t j = 1; j < N / 2; ++j)
						{
							reconstructed[i] += amp[j] * cos((double)i * j * 2 * pi / N + phase[j]);
						}
					}*/
				}
				DrawGL();
				SwapBuffers(hDC);
			}
			if (keys[VK_ESCAPE])
			{
				bQuit = true;
			}
		}
	}
	if(!test) MyMicrophone->Stop();
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
	//delete[] nums;
	//delete[] reconstructed;
	if (!test)
	{
		delete[] update_texture;
		delete[] pallet_blue;
		delete[] pallet_green;
		delete[] pallet_red;
		delete[] result;
		delete[] amp;
		delete[] phase;
		//delete[] input;
		delete[] sound;
		delete MyMicrophone;
		AudioLayer::Cleanup();
	}
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
	reconstruct_samples = 1;
	if (test)
	{
		fft.Initialize(P);
		//nums = new float[N]();
		nums.Initialize(P);
		reconstructed = new float[N]();
		size_t half = N / 2;
		amp = new float[half];
		phase = new float[half];
		float t = 0;
		srand(time(0));
		for (size_t i = 0; i < N; ++i, t += 2 * pi / (float)N) // Input signal
		{
			/*nums[i] = cos(30 * t) * 20;// +cos(300 * t) * 18;// +cos(25 * t / rad) * 75;
			//nums[i] = sqrt(t) * 30;// +sin(34 * t / rad) * 30;
			//nums[i] = 100 * ((i * 8 / N) % 2);
			//nums[i] = rand() % 100;*/
			nums.push_back(cos(440.5 * t) * 20/*+cos(460.1 * t) * 18/* +cos(25 * t / rad) * 75*/);
			//nums.push_back(sqrt(t) * 30;// +sin(34 * t / rad) * 30);
			//nums.push_back(100 * ((i * 8 / N) % 2));
			//nums.push_back(rand() % 100);
		}
		result = new Complex<float>[N];
		fft.Transform(nums, result);
		for (size_t i = 0; i < half; ++i)
		{
			amp[i] = result[i].mod() * 2;
			phase[i] = result[i].arg();
		}
		/*for (size_t i = 0; i < N; ++i)
		{
			reconstructed[i] += amp[0] * 0.5;
			for (size_t j = 1; j < half; ++j)
			{
				reconstructed[i] += amp[j] * cos((double)i * j * 2 * pi / N + phase[j]);
			}
		}*/
		//delete[] amp;
		//delete[] phase;
	}

	GLubyte* __data = new GLubyte[width * N * 2]();
	for (int i = 0; i < N / 2; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//__data[(i * width + j) * 4 + 0] = 0;
			__data[(i * width + j) * 4 + 1] = 255 * ((float)j / (float)width);
			__data[(i * width + j) * 4 + 2] = 255 * ((float)j / (float)width);
			__data[(i * width + j) * 4 + 3] = 255;
		}
	}
	glGenTextures(1, &spectrogram);
	glBindTexture(GL_TEXTURE_2D, spectrogram);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, N / 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, __data);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	delete[] __data;
}
void DrawGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spectrogram);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(tex_x, 0); glVertex2f(-width / 2, -height / 2);
	glTexCoord2f(tex_x - 1, 0); glVertex2f(width / 2, -height / 2);
	glTexCoord2f(tex_x - 1, 1); glVertex2f(width / 2, height / 2);
	glTexCoord2f(tex_x, 1); glVertex2f(-width / 2, height / 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	float x = -(float)N / 2.0;
	float scale = (float)width / (float)N;
	if (test)
	{
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for (size_t i = 0; i < N; ++i, x += 1)
		{
			glVertex2d(x * scale, nums[i]);
		}
		glEnd();
	}
	else
	{
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for (size_t i = 0; i < N; ++i, x += 1)
		{
			glVertex2d(x * scale, input[i] * 500 - height / 5);
		}
		glEnd();
	}
	x = 0.0;
	glColor3f(0, 0.6, 1);
	glBegin(GL_LINE_STRIP);
	//glBegin(GL_LINES);
	for (size_t i = 0; i < N / 2; ++i, x += 1.0 * scale_correction)
	{
		glVertex2d(x - width / 2, amp[i] * 100 + height / 5);
		//glVertex2d(x - width / 2, height / 5);
	}
	glEnd();
	/*
	x = -(float)N / 2.0;
	glColor3f(1, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < N; ++i, x += 1.0)
	{
		glVertex2d(x * scale, reconstructed[i]);
	}
	glEnd();
	*/
	glBegin(GL_LINES);
	for (float i = -width / 2; i < width / 2; i += 50)
	{
		glVertex2d(i, height / 5 - 5);
		glVertex2d(i, height / 5 - 15);
	}
	glVertex2d(440 - width / 2, height / 5 - 5);
	glVertex2d(440 - width / 2, height / 5 - 25);
	glEnd();
	if (!test)
	{
		glEnable(GL_TEXTURE_2D);
		unsigned char* freq = make_number_to_text((float)max_freq * (float)scale_correction);
		glScalef(1, -1, 1);
		glColor3f(1, 1, 1);
		draw_text(freq, -width / 2 + 50, -height / 2 + 50, 1);
		glScalef(1, -1, 1);
		delete[] freq;
		glDisable(GL_TEXTURE_2D);
	}
}
