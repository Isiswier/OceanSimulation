#include "simulator.h"
#include <sstream>
#include <cstdlib>
#include <vector>

extern float HEIGHTSCALE = INIT_HEIGHT;

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	float appA = DEFAULT_A;
	vector2 appWind = DEFAULT_WIND;
	
	const char* cmdstr = GetCommandLine();
	std::stringstream ss(cmdstr);
	std::string t;
	std::vector<std::string> paras;
	while (ss >> t)
		paras.push_back(t);
	
	if (paras.size() > 1) {
		appA = atof(paras[1].c_str());
		appWind = vector2(
			atof(paras[2].c_str()),
			atof(paras[3].c_str()));
	}
	

	Simulator oceanSimulation(hinstance);

	Simulator::PARAMS p;
	p.screenwidth  = 1200;
	p.screenHeight = 700;
	p.nVertsPerRow = MESH_N;
	p.nVertsPerCol = MESH_N;
	p.heightScale  = HEIGHTSCALE;
	p.oceanPara    = Ocean::OceanPara(appA, appWind);
	p.meshType	   = MeshPlane::MeshType::OCEAN;

	if (!oceanSimulation.Init(p))	return 0;
	oceanSimulation.Run();

	return 0;
}