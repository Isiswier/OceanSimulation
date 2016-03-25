#ifndef _FONT_H
#define _FONT_H

#include "d3dUtility.h"

class CFont{
public:	
	CFont(IDirect3DDevice9* dev) : FPS(0), frames(0), frameTime(0) { m_device = dev; }
	
	bool Init();
	void Draw(const char* str);
	void InfoFPS(float t);

private:

	float			  FPS;
	float			  frameTime;
	long			  frames;
	char			  buff[200];
	ID3DXFont*		  m_font = 0;
	IDirect3DDevice9* m_device = 0;
};

#endif