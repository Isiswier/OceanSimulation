#include "font.h"

bool CFont::Init()
{
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

void CFont::Draw(const char* str)
{
	RECT rect = {20, 20, 400, 200};
	m_font->DrawTextA(NULL, str, -1, &rect, DT_TOP | DT_LEFT, d3d::YELLOW);
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

	sprintf(buff, "FPS:     %f", FPS);
	Draw(buff);
}