#ifndef _FONT_H
#define _FONT_H

#include "d3dUtility.h"

class CFont{
public:	
	CFont(IDirect3DDevice9* dev) : FPS(0), frames(0), frameTime(0) { m_device = dev; }
	
	bool Init(float dispA, float dispWindX, float dispWindY);
	void InfoFPS(float t);

private:

	float			  FPS;
	float			  frameTime;
	long			  frames;
	char			  buff[200];
	float			  A, windX, windY;
	RECT			  rect;
	ID3DXFont*		  m_font = 0;
	IDirect3DDevice9* m_device = 0;

	inline void dispInfo() {
		sprintf(buff, "FPS:     %f\nA:         %f\nWind: (%.1f, %.1f)\nHeightScale: %.2f",
				FPS, A, windX, windY, HEIGHTSCALE);
		m_font->DrawTextA(NULL, buff, -1, &rect, DT_TOP | DT_LEFT, d3d::YELLOW);
	}
};

#endif