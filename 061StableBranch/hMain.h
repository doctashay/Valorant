#ifndef HMAIN_H
#define HMAIN_H

#include "hDirectX.h"

#include <Windows.h>
#include <iostream>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
extern HWND tWnd;
extern HANDLE hProcess;
int DrawText(char* String, int x, int y, DWORD color);

#endif
