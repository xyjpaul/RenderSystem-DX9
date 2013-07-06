#include "InterfaceFunctions.h"

int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!RenderSystem::getSingleton()->InitD3D(hinstance,true,D3DDEVTYPE_HAL))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!PreRender())
	{
		::MessageBox(0, "PreRender() - FAILED", 0, 0);
		return 0;
	}

	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));
	static float lastTime = (float)timeGetTime(); 

	//进入消息循环
	while(msg.message != WM_QUIT)
	{
		if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
        {	
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;
			BeginRender(timeDelta);
			lastTime = currTime;
        }
    }
	//清除渲染系统
	RenderSystem::deleteSingleton();
	return 0;
}


	
