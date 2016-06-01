#pragma once
#include <windows.h>
#include <gl/gl.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void CreateOGLWindow(int, int, bool);
void GetMonitorParameters(int&, int&);
void SetVSync(bool);
void DrawGL();
