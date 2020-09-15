#include "hDrawings.h"
ID3DXFont* pFont = 0;
#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
int DrawString(const char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
{
	RECT ShadowPos;
	ShadowPos.left = x + 1;
	ShadowPos.top = y + 1;
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	ifont->DrawTextA(0, String, strlen(String), &ShadowPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r / 3, g / 3, b / 3));
	ifont->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
}

int DrawShadowString(const char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
{
	RECT Font;
	Font.left = x;
	Font.top = y;
	RECT Fonts;
	Fonts.left = x + 1;
	Fonts.top = y;
	RECT Fonts1;
	Fonts1.left = x - 1;
	Fonts1.top = y;
	RECT Fonts2;
	Fonts2.left = x;
	Fonts2.top = y + 1;
	RECT Fonts3;
	Fonts3.left = x;
	Fonts3.top = y - 1;
	ifont->DrawTextA(0, String, strlen(String), &Fonts3, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts2, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts1, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
}

void DrawFilledBox(int x, int y, int w, int h, int r, int g, int b, int a)
{
	static ID3DXLine* pLine;

	if (!pLine)
		D3DXCreateLine(p_Device, &pLine);

	D3DXVECTOR2 vLine[2];

	pLine->SetWidth(w);
	pLine->SetAntialias(false);
	pLine->SetGLLines(true);

	vLine[0].x = x + w / 2;
	vLine[0].y = y;
	vLine[1].x = x + w / 2;
	vLine[1].y = y + h;

	pLine->Begin();
	pLine->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
	pLine->End();
}

void DrawBox(int x, int y, int width, int height, int px, int r, int g, int b, int a)
{
	DrawFilledBox(x, y + height, width, px, r, g, b, a);
	DrawFilledBox(x - px, y, px, height, r, g, b, a);
	DrawFilledBox(x, y - px, width, px, r, g, b, a);
	DrawFilledBox(x + width, y, px, height, r, g, b, a);
}

int DrawText(char* String, int x, int y, DWORD color)
{
	RECT Font;
	Font.bottom = 0;
	Font.left = x;
	Font.top = y;
	Font.right = 0;

	pFont->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, color);

	return 0;
}

struct D3DTLVERTEX
{
	float X, Y, Z, RHW;
	DWORD Color;
};

void DrawLine(float startX, float startY, float endX, float endY, D3DCOLOR dwColour)
{
	D3DTLVERTEX Vertex[2] = {
		{ (float)startX, (float)startY, 0.0f, 1.0f, dwColour },
		{ (float)endX, (float)endY, 1.0f, 1.0f, dwColour },
	};

	const DWORD FVFTL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	IDirect3DBaseTexture9* pTexture = NULL;

	DWORD dwLighting = 0, dwZEnable = 0, dwZFunc = 0, dwFVF = 0, dwColorOp = 0, dwColorArg = 0;

	p_Device->GetTexture(0, &pTexture);
	p_Device->GetRenderState(D3DRS_LIGHTING, &dwLighting);
	p_Device->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	p_Device->GetRenderState(D3DRS_ZFUNC, &dwZFunc);
	p_Device->GetFVF(&dwFVF);
	p_Device->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
	p_Device->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg);

	p_Device->SetTexture(0, NULL);
	p_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	p_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	p_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	p_Device->SetFVF(FVFTL);
	p_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	p_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	p_Device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &Vertex, sizeof(D3DTLVERTEX));

	p_Device->SetTexture(0, pTexture);
	p_Device->SetRenderState(D3DRS_LIGHTING, dwLighting);
	p_Device->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	p_Device->SetRenderState(D3DRS_ZFUNC, dwZFunc);
	p_Device->SetFVF(dwFVF);
	p_Device->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
	p_Device->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg);
}

void FillRGB(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice)
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

struct TEXTUREVERTEX
{
	D3DXVECTOR3 vec;
	FLOAT rhw;
	DWORD color;
	FLOAT u, v;
};

void CriarVertex(TEXTUREVERTEX* pVertices, FLOAT x, FLOAT y, FLOAT u, FLOAT v, DWORD dwCor)
{
	pVertices->vec = D3DXVECTOR3(x - 0.5f, y - 0.5f, 0);
	pVertices->rhw = 1.0f;
	pVertices->u = u;
	pVertices->v = v;
	pVertices->color = dwCor;
}

void DrawRect(int x, int y, int h, int w, DWORD Color, LPDIRECT3DDEVICE9 pDevice) {
	D3DRECT rec;
	rec.x1 = x;
	rec.x2 = x + w;
	rec.y1 = y;
	rec.y2 = y + h;
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, Color, 0, 0);
}

VOID DrawBox2(INT x, INT y, INT h, INT w, D3DCOLOR Border, D3DCOLOR Fill, LPDIRECT3DDEVICE9 pDevice)
{
	DrawRect(x, y, h, w, Fill, pDevice);
	DrawRect(x, y, h, 1, Border, pDevice);
	DrawRect(x + w, y, h, 1, Border, pDevice);
	DrawRect(x, y, 1, w, Border, pDevice);
	DrawRect(x, y + h, 1, w + 1, Border, pDevice);
	DrawRect(x + 2, y + 2, h - 4, 2, Border, pDevice);
	DrawRect(x + 2, y + 2, 2, w - 4, Border, pDevice);
	DrawRect(x + w - 4, y + 2, h - 4, 2, Border, pDevice);
	DrawRect(x + 2, y + h - 4, 2, w - 4, Border, pDevice);
}

void DrawBox(int x, int y, int w, int h, DWORD ColorStart, int UsarVertex1, LPDIRECT3DDEVICE9 m_pd3dDevice)
{
	RECT rect = { x, y, x + w, y + h };;

	FLOAT Esquerda = (FLOAT)rect.left;
	FLOAT Cima = (FLOAT)rect.top;
	FLOAT Direita = (FLOAT)rect.right;
	FLOAT Baixo = (FLOAT)rect.bottom;

	TEXTUREVERTEX vertex[4];
	TEXTUREVERTEX* pVertices = vertex;

	CriarVertex(pVertices, Esquerda, Cima, rect.left, rect.left, ColorStart);
	pVertices++;
	CriarVertex(pVertices, Direita, Cima, rect.right, rect.right, ColorStart);
	pVertices++;
	CriarVertex(pVertices, Esquerda, Baixo, rect.bottom, rect.bottom, ColorStart);
	pVertices++;
	CriarVertex(pVertices, Direita, Baixo, rect.top + rect.right, rect.top + rect.right, ColorStart);
	pVertices++;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);


	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetTexture(1, NULL);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX));
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetTexture(1, NULL);
}

void DrawMouse(LPDIRECT3DDEVICE9 pDevice)
{
	POINT myCursor;
	GetCursorPos(&myCursor);
	DrawBox(myCursor.x, myCursor.y, 10, 10, D3DCOLOR_XRGB(65, 65, 65), D3DCOLOR_XRGB(0, 0, 0), pDevice);//Mouse Drawn
}