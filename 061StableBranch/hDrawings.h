#ifndef HDRAWFUNC_H
#define HDRAWFUNC_H

#include "hDirectX.h"

#pragma warning( disable : 4244 ) //remove the incoming warns
#pragma warning( disable : 4996 ) //remove the incoming warns

#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

int DrawString(const char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont);
int DrawShadowString(const char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont);
void DrawFilledBox(int x, int y, int w, int h, int r, int g, int b, int a);
void DrawBox(int x, int y, int width, int height, int px, int r, int g, int b, int a);
void DrawLine(float startX, float startY, float endX, float endY, D3DCOLOR dwColour);
void FillRGB(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice);
void DrawRect(int x, int y, int h, int w, DWORD Color, LPDIRECT3DDEVICE9 pDevice);
void DrawBox2(INT x, INT y, INT h, INT w, D3DCOLOR Border, D3DCOLOR Fill, LPDIRECT3DDEVICE9 pDevice);
void DrawBox(int x, int y, int w, int h, DWORD ColorStart, int UsarVertex1, LPDIRECT3DDEVICE9 m_pd3dDevice);
int DrawText(char* String, int x, int y, DWORD color);
void DrawMouse(LPDIRECT3DDEVICE9 pDevice);

#define Red						D3DCOLOR_ARGB(255, 255, 0, 0)
#define DarkRed					D3DCOLOR_ARGB(255, 200, 0, 0)
#define Green					D3DCOLOR_ARGB(255, 127, 255, 0)
#define Orange					D3DCOLOR_ARGB(255, 0, 255, 255)
#define Blue					D3DCOLOR_ARGB(255, 0, 255, 255)
#define Yellow					D3DCOLOR_ARGB(255, 255, 255, 51)
#define Black					D3DCOLOR_ARGB(255, 0, 0, 0)
#define Grey					D3DCOLOR_ARGB(255, 112, 112, 112)
#define Gold					D3DCOLOR_ARGB(255, 255, 215, 0)
#define Pink					D3DCOLOR_ARGB(255, 255, 192, 203)
#define Purple					D3DCOLOR_ARGB(255, 128, 0, 128)
#define White					D3DCOLOR_ARGB(255, 255, 255, 255)
#define Cyan					D3DCOLOR_ARGB(255, 0, 255, 255)
#define Magenta					D3DCOLOR_ARGB(255, 255, 0, 255)
#define TBlack					D3DCOLOR_ARGB(200, 0, 0, 0)
#define DodgerBlue				D3DCOLOR_ARGB(255, 30, 144, 255)
#endif