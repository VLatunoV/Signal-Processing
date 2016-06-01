#include "gl_construct.h"
#include <iostream>
#include "fourier.h"
#include "audio_layer.h"
#include "font.h"
#include <time.h>

using namespace std;

// Microphone Capture
AudioLayer::CaptureEndpoint* MyMicrophone;
BYTE* soundArray = nullptr;
size_t soundLength;
float samplingIndex = 0.0;
float scaling;
UINT32 timeInterval;

// Fourier Transform
FFT fft;
size_t N;
size_t P;
size_t maxFrequency = 0;

// Input signal
Cyclic<float> genInput;
Cyclic<float> capInput;
Complex<float>* result = nullptr;
float* reconstructed = nullptr;
float reconstructSamples;
float* amp = nullptr;
float* phase = nullptr;
bool generated = false;				// True -> generated input signal, False -> capture input signal

// Spectrogram
unsigned int spectroID;
int spectroWidth;
float spectroOffsetX = 1;
unsigned char* spectroUpdateArray = nullptr;
unsigned char* palletRed = nullptr;
unsigned char* palletGreen = nullptr;
unsigned char* palletBlue = nullptr;
const size_t palletSize = 200;

// Window
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

void Initialize()
{
	P = 13;
	N = (size_t)1 << P;
	fft.Initialize(P);
	amp = new float[N / 2];
	phase = new float[N / 2];
	result = new Complex<float>[N];

	if (!generated)
	{
		Initialize_Font();
		MyMicrophone = AudioLayer::GetDefaultCaptureEndpoint();
		//timeInterval = N * 1000 / MyMicrophone->GetWaveFormat()->nSamplesPerSec;
		timeInterval = 20;
		MyMicrophone->Initialize(timeInterval);
		soundLength = MyMicrophone->GetBufferSize();
		scaling = (float)MyMicrophone->GetWaveFormat()->nSamplesPerSec / (float)N;

		MyMicrophone->Start();
		soundArray = new BYTE[soundLength]();
		spectroUpdateArray = new unsigned char[4 * N / 2];
		palletBlue = new unsigned char[palletSize];
		palletGreen = new unsigned char[palletSize];
		palletRed = new unsigned char[palletSize];
		float I;
		for (int index = 0; index < palletSize; ++index)
		{
			I = (float)index * 255 / (float)palletSize;
			palletRed[index] = I;
			palletGreen[index] = 0;
			palletBlue[index] = 0;
		}
		for (int index = 0; index < N / 2; ++index)
		{
			spectroUpdateArray[index * 4 + 3] = 255;
		}
		capInput.Initialize(P);
	}
	else
	{
		reconstructSamples = 1.f;
		genInput.Initialize(P);
		reconstructed = new float[N]();
		size_t half = N / 2;
		float t = 0.f;
		srand(time(0));
		for (size_t i = 0; i < N; ++i, t += 2 * pi / (float)N)	// Input signal
		{
			/*genInput[i] = cos(30 * t) * 20;// +cos(300 * t) * 18;// +cos(25 * t / rad) * 75;
			//genInput[i] = sqrt(t) * 30;// +sin(34 * t / rad) * 30;
			//genInput[i] = 100 * ((i * 8 / N) % 2);
			//genInput[i] = rand() % 100;*/
			genInput.push_back(cos(440.5 * t) * 20/*+cos(460.1 * t) * 18/* +cos(25 * t / rad) * 75*/);
			//genInput.push_back(sqrt(t) * 30;// +sin(34 * t / rad) * 30);
			//genInput.push_back(100 * ((i * 8 / N) % 2));
			//genInput.push_back(rand() % 100);
		}
		fft.Transform(genInput, result);
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
	}
	spectroWidth = width;
	GLubyte* __data = new GLubyte[spectroWidth * (N / 2) * 4]();
	glGenTextures(1, &spectroID);
	glBindTexture(GL_TEXTURE_2D, spectroID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, spectroWidth, N / 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, __data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	delete[] __data;
}
void CleanUp()
{
	if (!generated)
	{
		delete[] spectroUpdateArray;
		delete[] palletBlue;
		delete[] palletGreen;
		delete[] palletRed;
		delete[] soundArray;
		MyMicrophone->Stop();
		delete MyMicrophone;
		AudioLayer::Cleanup();
	}
	else
	{
		delete[] reconstructed;
	}
	delete[] result;
	delete[] amp;
	delete[] phase;
}
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
	Initialize();
	
	int pickColor;

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
				if(!generated)
				{
					MyMicrophone->Capture(timeInterval, soundArray);
					int i;
					for (i = 0; samplingIndex < soundLength / 8; samplingIndex += scaling, ++i)
					{
						capInput.push_back(((float*)soundArray)[int(samplingIndex) * 2]);
						//capInput.push_back((float)(((float*)soundArray)[i]));
					}
					//std::cout << "Samples: " << i << "\n";
					samplingIndex -= soundLength / 8;
					if (capInput.GetSize() == N)
					{
						fft.Transform(capInput, result);
						maxFrequency = 0;
						for (UINT32 i = 0; i < N / 2; ++i)
						{
							//amp[i] = result[i].mod() * 2 * 1000;
							amp[i] = result[i].mod_sqr() * N * 100;
							//phase[i] = result[i].arg();
							if (amp[i] > amp[maxFrequency])
							{
								maxFrequency = i;
							}
						}
						for (i = 0; i < N / 2; ++i)
						{
							pickColor = (amp[i] * palletSize);
							pickColor = pickColor > palletSize - 1 ? palletSize - 1 : pickColor;
							spectroUpdateArray[i * 4] = palletRed[pickColor];
							spectroUpdateArray[i * 4 + 1] = palletGreen[pickColor];
							spectroUpdateArray[i * 4 + 2] = palletBlue[pickColor];
						}
						glBindTexture(GL_TEXTURE_2D, spectroID);
						glTexSubImage2D(GL_TEXTURE_2D, 0, (int)(spectroOffsetX * spectroWidth), 0, 1, N / 2, GL_RGBA, GL_UNSIGNED_BYTE, spectroUpdateArray);
						spectroOffsetX += 1.f / spectroWidth;
						if (spectroOffsetX > 1.0)
						{
							spectroOffsetX -= 1.0;
						}

					}
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
	CleanUp();
	DisableOpenGL(hWnd, hDC, hRC);
	DestroyWindow(hWnd);
	return msg.wParam;
}
void DrawGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	float x = -(float)N / 2.f;
	float scale = (float)width / (float)N;

	// Draw the spectrogram
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spectroID);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(spectroOffsetX, 0); glVertex2f(-width / 2, -height / 2);
	glTexCoord2f(spectroOffsetX - 1, 0); glVertex2f(width / 2, -height / 2);
	glTexCoord2f(spectroOffsetX - 1, 1); glVertex2f(width / 2, height / 2);
	glTexCoord2f(spectroOffsetX, 1); glVertex2f(-width / 2, height / 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// Draw the input
	if (generated)
	{
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for (size_t i = 0; i < N; ++i, x += scale)
		{
			glVertex2d(x, genInput[i] - height / 5);
		}
		glEnd();
	}
	else
	{
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for (size_t i = 0; i < N; ++i, x += 1)
		{
			glVertex2d(x * scale, capInput[i] * 500 - height / 5);
		}
		glEnd();
	}

	// Draw the frequency decomposition
	x = 0.0;
	glColor3f(0, 0.6, 1);
	bool line_strip = true;
	if(line_strip)
		glBegin(GL_LINE_STRIP);
	else
		glBegin(GL_LINES);
	for (size_t i = 0; i < N / 2; ++i, x += 1.0)
	{
		glVertex2d(x - width / 2, amp[i] * 100 + height / 5);
		if (!line_strip)
			glVertex2d(x - width / 2, height / 5);
	}
	glEnd();

	/*
	// Draw the reconstructed signal
	x = -(float)N / 2.0;
	glColor3f(1, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < N; ++i, x += scale)
	{
		glVertex2d(x, reconstructed[i]);
	}
	glEnd();
	*/

	// Draw guide lines every 50 units
	glBegin(GL_LINES);
	for (float i = -width / 2; i < width / 2; i += 50)
	{
		glVertex2d(i, height / 5 - 5);
		glVertex2d(i, height / 5 - 15);
	}
	glVertex2d(440 - width / 2, height / 5 - 5);
	glVertex2d(440 - width / 2, height / 5 - 25);
	glEnd();

	// Draw frequency text
	if (!generated)
	{
		glEnable(GL_TEXTURE_2D);
		unsigned char* freq = make_number_to_text((float)maxFrequency);
		glScalef(1, -1, 1);
		glColor3f(1, 1, 1);
		draw_text(freq, -width / 2 + 50, -height / 2 + 50, 1);
		glScalef(1, -1, 1);
		delete[] freq;
		glDisable(GL_TEXTURE_2D);
	}
}
