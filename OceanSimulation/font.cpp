#include "font.h"

bool CFont::Init(float dispA, float dispWindX, float dispWindY)
{
	A = dispA;
	windX = dispWindX;
	windY = dispWindY;
	rect = { 20, 20, 400, 200 };

	D3DXFONT_DESC df;
	ZeroMemory(&df, sizeof(D3DXFONT_DESC));
	df.Height = 16;
	df.Width = 7;
	df.Weight = 0;
	df.MipLevels = D3DX_DEFAULT;
	df.Italic = false;
	df.CharSet = DEFAULT_CHARSET;
	df.OutputPrecision = 0;
	df.Quality = 0;
	df.PitchAndFamily = 0;
	strcpy((char*)df.FaceName, "calibri");

	HRESULT hr = D3DXCreateFontIndirect(m_device, &df, &m_font);
	if (FAILED(hr))
		return false;
	return true;
}

void CFont::InfoFPS(float t)
{	
	frames++;
	frameTime += t;

	if (frameTime > 1.0f) {
		FPS = frames / frameTime;
		frames = 0;
		frameTime = 0;
	}

	dispInfo();
}