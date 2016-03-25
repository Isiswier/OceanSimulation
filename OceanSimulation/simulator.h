#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include "d3dUtility.h"
#include "camera.h"
#include "meshPlane.h"
#include "font.h"

class Simulator{
public:
	struct PARAMS {
		int	 screenwidth;
		int	 screenHeight;
		int	 meshType;
		int	 nVertsPerRow;
		int	 nVertsPerCol;
		int	 cellSpacing;
		float	heightScale;
		const char*		  dataFileName = 0;
		Ocean::OceanPara  oceanPara;
	};

	Simulator(HINSTANCE hInst) { m_hInst = hInst; }
	~Simulator();
	
	int			Run();
	bool		Init(PARAMS p);

private:

	HINSTANCE		  m_hInst;
	IDirect3DDevice9* m_Device    = 0;
	MeshPlane*		  m_MeshPlane = 0;
	Camera			  m_Camera;
	CFont*			  m_Display;
	void Disp(float timeDelta);
};

#endif