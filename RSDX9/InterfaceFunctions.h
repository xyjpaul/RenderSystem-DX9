#include "RenderSystem.h"

#define XFILE_NAME		  TEXT("Hall.x")
#define LOADING_TEX_NAME  TEXT("colorloading.jpg")
#define FONT_STYLE		  TEXT("Times New Roman")
#define HELP_TEXT		  TEXT("按下W,S,A,D进行前后左右移动,\n使用方向键调整视角.\n按下F1键关闭提示")
#define HELP_TEXT_SHOW	  TEXT("按下F1键开启提示.")

#ifndef __IFH__
#define __IFH__

bool PreRender()
{
	IDirect3DDevice9 *Device = RenderSystem::getSingleton()->getDevice();
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f,
		(float)RenderSystem::getSingleton()->getWidth() / (float)RenderSystem::getSingleton()->getHeight(),
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	
	D3DXMATRIX V;

	D3DXVECTOR3 position(0.0f,0.0f,-4.0f);
	D3DXVECTOR3 target(0.0f,0.0f,0.0f);
	D3DXVECTOR3 up(0.0f,1.0f,0.0f);

	D3DXMatrixLookAtLH(&V,&position,&target,&up);
	Device->SetTransform(D3DTS_VIEW, &V);
	Device->SetRenderState(D3DRS_LIGHTING, false);

	IDirect3DVertexBuffer9* loading  = 0;
	IDirect3DTexture9*      tex    = 0;
	

	
	Device->CreateVertexBuffer(
			6 * sizeof(Vertex),
			0, 
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&loading,
			0);
		
		Vertex* v = 0;
		loading->Lock(0, 0, (void**)&v, 0);
		v[0] = Vertex(-2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-2.0f,  2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex( 2.0f,  2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		
		v[3] = Vertex(-2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[4] = Vertex( 2.0f,  2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[5] = Vertex( 2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		loading->Unlock();
		
		D3DXCreateTextureFromFile(
			Device,
			LOADING_TEX_NAME,
			&tex);

		Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		
		
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		Device->SetTexture(0, tex);
		Device->SetStreamSource(0, loading, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);

		if(!RenderSystem::getSingleton()->LoadMesh(XFILE_NAME))
		return false;

		RenderSystem::getSingleton()->SetFont(30,15,500,FONT_STYLE);
		Device->SetRenderState(D3DRS_LIGHTING, true);
		Device->SetTexture(0, NULL);
		RenderSystem::getSingleton()->yaw(0.8f);
		RenderSystem::getSingleton()->walk(-8.0f);
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
		
		Device->SetLight(0, &light);
		Device->LightEnable(0, true);
		Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		Device->SetRenderState(D3DRS_SPECULARENABLE, true);
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();
		
		//显示帮助
		if(!RenderSystem::getSingleton()->getOff())
			RenderSystem::getSingleton()->ShowHelp(HELP_TEXT);
		else
			RenderSystem::getSingleton()->ShowHelp(HELP_TEXT_SHOW);

		D3DXMATRIX T, R, P, S;

		D3DXMatrixScaling(&S, 0.003f, 0.003f, 0.003f);
		D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);
		D3DXMatrixTranslation(&P, -5.0f, -2.08f, -10.0f + (1 * 7.5f));
		D3DXMatrixIdentity(&T);
		T = T * S * P* R;
		
		Device->SetTransform(D3DTS_WORLD, &T);

		RenderSystem::getSingleton()->DrawMesh();

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}
#endif