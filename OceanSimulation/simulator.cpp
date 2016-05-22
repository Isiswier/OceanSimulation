#include "simulator.h"

bool Simulator::Init(PARAMS p)
{
	// Init D3D
	if (!d3d::InitD3D(m_hInst, p.screenwidth, p.screenHeight, true, D3DDEVTYPE_HAL, &m_Device)) {
		MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return false;
	}

	// Create MeshPlane
	m_MeshPlane = new MeshPlane(
		m_Device, 
		p.dataFileName, 
		p.nVertsPerRow, 
		p.nVertsPerCol, 
		p.cellSpacing, 
		p.heightScale, 
		p.meshType, 
		p.oceanPara);

	// Create Font Display
	m_Display = new CFont(m_Device);
	if (!m_Display->Init(p.oceanPara.A, p.oceanPara.w.x, p.oceanPara.w.y))
		return false;

	// Set Perspective View
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH( &proj, D3DX_PI * 0.25f, (float)p.screenwidth / p.screenHeight, 1.0f, 3000.0f);
	m_Device->SetTransform(D3DTS_PROJECTION, &proj);

	//m_Camera.roll(D3DXToRadian(180));
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = D3DXCOLOR(0x223c67);
	mtrl.Diffuse = d3d::WHITE * 0.3f;
	mtrl.Specular = d3d::BLACK;
	mtrl.Emissive = d3d::BLACK;
	mtrl.Power = 5.0f;
	m_Device->SetMaterial(&mtrl);

	D3DLIGHT9 dir;
	ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = D3DXCOLOR(0xe4e0af);
	dir.Specular = D3DXCOLOR(0xb3e6b9)* 0.3f;
	dir.Ambient = d3d::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	m_Device->SetLight(0, &dir);
	m_Device->LightEnable(0, true);

	m_Device->SetRenderState(D3DRS_LIGHTING, true);
	m_Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}

Simulator::~Simulator() {
	d3d::Release<IDirect3DDevice9*>(m_Device);
	d3d::Delete<MeshPlane*>(m_MeshPlane);
}

int Simulator::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	static const float firstTime = (float)timeGetTime();
	static float lastTime = firstTime;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			Disp(timeDelta);

			lastTime = currTime;
		}
	}
	return msg.wParam;
}

void Simulator::Disp(float timeDelta)
{
	static float elapsedTime = 0;

	if (m_Device)
	{
		if (GetAsyncKeyState('W') & 0x8000f)
			m_Camera.walk(100.0f * timeDelta);

		if (GetAsyncKeyState('S') & 0x8000f)
			m_Camera.walk(-100.0f * timeDelta);

		if (GetAsyncKeyState('A') & 0x8000f)
			m_Camera.strafe(-100.0f * timeDelta);

		if (GetAsyncKeyState('D') & 0x8000f)
			m_Camera.strafe(100.0f * timeDelta);

		if (GetAsyncKeyState('E') & 0x8000f)
			m_Camera.fly(50.0f * timeDelta);

		if (GetAsyncKeyState('Q') & 0x8000f)
			m_Camera.fly(-50.0f * timeDelta);

		if (GetAsyncKeyState('R') & 0x8000f)
			m_Camera.roll(5.0f * timeDelta);

		if (GetAsyncKeyState('F') & 0x8000f)
			m_Camera.roll(-5.0f * timeDelta);

		if (GetAsyncKeyState('H') & 0x8000f)
			HEIGHTSCALE += 0.005f;

		if (GetAsyncKeyState('J') & 0x8000f)
			HEIGHTSCALE -= 0.005f;


		if (GetAsyncKeyState(VK_LEFT) & 0x8000f)
			m_Camera.yaw(-1.0f * timeDelta);

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			m_Camera.yaw(1.0f * timeDelta);

		if (GetAsyncKeyState(VK_UP) & 0x8000f)
			m_Camera.pitch(-1.0f * timeDelta);

		if (GetAsyncKeyState(VK_DOWN) & 0x8000f)
			m_Camera.pitch(1.0f * timeDelta);



		D3DXMATRIX V;
		m_Camera.getViewMatrix(&V);
		m_Device->SetTransform(D3DTS_VIEW, &V);


		m_Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
		m_Device->BeginScene();

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);

		if (m_MeshPlane)
			m_MeshPlane->draw(&I, elapsedTime * TIME_RATIO);
		elapsedTime += timeDelta;
		
		m_Display->InfoFPS(timeDelta);

		m_Device->EndScene();
		m_Device->Present(0, 0, 0, 0);
	}
}