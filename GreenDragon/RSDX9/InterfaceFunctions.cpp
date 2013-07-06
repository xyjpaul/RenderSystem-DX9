#include "InterfaceFunctions.h"

CFBXLoader	g_FBXLoader;

bool PreRender()
{
	IDirect3DDevice9 *Device = RenderSystem::getSingleton()->getDevice();
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f,
		(float)RenderSystem::getSingleton()->getWidth() / (float)RenderSystem::getSingleton()->getHeight(),
		0.1f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	//g_FBXLoader.Initialize();
	//g_FBXLoader.LoadFile("123.fbx");
	//g_FBXLoader.FbxObjectContent();
	char fileName[MAX_PATH];
	g_FBXLoader.LoadFile();	
	g_FBXLoader.FbxObjectContent();
	g_FBXLoader.GetFileName(fileName);

	IDirect3DTexture9* tex = 0;

	D3DXCreateTextureFromFile(
		Device,
		fileName,
		&tex);
	Device->SetTexture(0,tex);
	RenderSystem::getSingleton()->SetFont(30,15,500,FONT_STYLE);
	return true;
}

bool BeginRender(float timeDelta)
{
	IDirect3DDevice9 *Device = RenderSystem::getSingleton()->getDevice();
	if(Device)
	{		
		//
		// 更新相机
		//
		if( ::GetAsyncKeyState('W') & 0x8000f )
			RenderSystem::getSingleton()->walk(4.0f * timeDelta);

		if( ::GetAsyncKeyState('S') & 0x8000f )
			RenderSystem::getSingleton()->walk(-4.0f * timeDelta);

		if( ::GetAsyncKeyState('A') & 0x8000f )
			RenderSystem::getSingleton()->strafe(-4.0f * timeDelta);

		if( ::GetAsyncKeyState('D') & 0x8000f )
			RenderSystem::getSingleton()->strafe(4.0f * timeDelta);

		if( ::GetAsyncKeyState('R') & 0x8000f )
			RenderSystem::getSingleton()->fly(4.0f * timeDelta);

		if( ::GetAsyncKeyState('F') & 0x8000f )
			RenderSystem::getSingleton()->fly(-4.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			RenderSystem::getSingleton()->pitch(1.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			RenderSystem::getSingleton()->pitch(-1.0f * timeDelta);

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			RenderSystem::getSingleton()->yaw(-1.0f * timeDelta);
			
		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			RenderSystem::getSingleton()->yaw(1.0f * timeDelta);

		if( ::GetAsyncKeyState('N') & 0x8000f )
			RenderSystem::getSingleton()->roll(1.0f * timeDelta);

		if( ::GetAsyncKeyState('M') & 0x8000f )
			RenderSystem::getSingleton()->roll(-1.0f * timeDelta);

		
		D3DXMATRIX V;
		RenderSystem::getSingleton()->getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		D3DXVECTOR3 dir(0.707f, -0.207f, 0.707f);
		D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
		D3DLIGHT9 light = RenderSystem::getSingleton()->InitDirectionalLight(&dir, &col);

		//Device->SetLight(0, &light);
		//Device->LightEnable(0, true);
		Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		Device->SetRenderState(D3DRS_LIGHTING, false);
		//Device->SetRenderState(D3DRS_SPECULARENABLE, true);
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();
		
		//显示帮助
		if(!RenderSystem::getSingleton()->getOff())
			RenderSystem::getSingleton()->ShowHelp(HELP_TEXT);
		else
			RenderSystem::getSingleton()->ShowHelp(HELP_TEXT_SHOW);

		D3DXMATRIX T,R,S;
		D3DXMatrixScaling(&S, 0.003f, 0.003f, 0.003f);
		//D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);
		D3DXMatrixIdentity(&T);
		T = T * S;

		Device->SetTransform(D3DTS_WORLD, &T);
		
		list<LPD3DXMESH>::iterator it = g_FBXLoader.m_D3DMeshList.begin();
		for(;it != g_FBXLoader.m_D3DMeshList.end();it++)
		{
			(*it)->DrawSubset(0);
		}
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}