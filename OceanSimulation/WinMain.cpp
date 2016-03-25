#include "simulator.h"

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	Simulator oceanSimulation(hinstance);

	Simulator::PARAMS p;
	p.screenwidth  = 1200;
	p.screenHeight = 700;
	p.nVertsPerRow = 32;
	p.nVertsPerCol = 32;
	p.cellSpacing  = 10;
	p.heightScale  = HEIGHTSCALE;
	p.dataFileName = NULL;
	//p.oceanPara    = Ocean::OceanPara(CUSTOM_A, CUSTOM_WIND);
	p.meshType	   = MeshPlane::MeshType::TERRAIN;

	if (!oceanSimulation.Init(p))	return 0;
	oceanSimulation.Run();

	return 0;
}