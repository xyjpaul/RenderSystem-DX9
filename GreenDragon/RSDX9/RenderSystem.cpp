#include "RenderSystem.h"

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
RenderSystem* RenderSystem::m_pRenderSystem = NULL;

RenderSystem::RenderSystem()
{
	m_pD3d9			= NULL;				
	m_pDevice		= NULL;		
	m_Vp			= 0;			
	m_HWnd	        = NULL;					
	m_pWindowName   = TEXT("Show");
	m_Width		    = 800;		
	m_Height		= 600;
	m_pFont			= 0;
	m_Off		    = false;
	m_pMaterialMesh = NULL;
	m_pCamera		= NULL;
}

RenderSystem::~RenderSystem()
{
	if(m_pCamera){delete m_pCamera;m_pCamera = NULL;}
	if(m_pMaterialMesh){delete m_pMaterialMesh;m_pMaterialMesh = NULL;}
	if(m_pFont){m_pFont->Release();m_pFont = NULL;}
	if(m_pDevice){m_pDevice->Release();m_pDevice = NULL;}
	if(m_pD3d9){m_pD3d9->Release();m_pD3d9 = NULL;}
}

RenderSystem* RenderSystem::getSingleton()
{
	if(!m_pRenderSystem)
	{
		m_pRenderSystem = new RenderSystem();
	}
	return m_pRenderSystem;
}

void RenderSystem::deleteSingleton()
{
	if(m_pRenderSystem){delete m_pRenderSystem;m_pRenderSystem = NULL;}
}

bool RenderSystem::InitD3D(
						   HINSTANCE hInstance,						
						   bool windowed, 
						   D3DDEVTYPE deviceType)
{
	//
	// 创建窗口类.
	//

	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = m_pWindowName;

	if( !RegisterClass(&wc) ) 
	{
		::MessageBox(0, TEXT("窗口注册失败"), 0, 0);
		return false;
	}
	
	m_HWnd = ::CreateWindow(m_pWindowName, m_pWindowName, 
		WS_OVERLAPPEDWINDOW,
		0, 0, m_Width, m_Height,
		0 /*父窗口句柄*/, 0 /* 菜单 */, hInstance, 0 /*其他*/); 

	if( !m_HWnd )
	{
		::MessageBox(0, TEXT("窗口创建失败"), 0, 0);
		return false;
	}

	::ShowWindow(m_HWnd, SW_SHOW);
	::UpdateWindow(m_HWnd);

	//获取窗口客户区大小
	GetClientRect(m_HWnd,&m_Rect);

	//
	// 初始化D3D: 
	//

	HRESULT hr = 0;

	// 1: 创建IDirect3D9对象.

    m_pD3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !m_pD3d9 )
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// 2: 检查硬件vp.
	
	m_pD3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &m_Caps);

	if( m_Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		m_Vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		m_Vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// 3: 填充D3DPRESENT_PARAMETERS结构体.
 
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = m_Width;
	d3dpp.BackBufferHeight           = m_Height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = m_HWnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// 4: 创建设备.

	hr = m_pD3d9->CreateDevice(
		D3DADAPTER_DEFAULT,  // 第一块显卡
		deviceType,			 // 设备类型
		m_HWnd,              // 设备相关的窗口句柄
		m_Vp,                // 顶点处理模式
	    &d3dpp,              // D3DPRESENT_PARAMETERS结构体
	    &m_pDevice);          // 输出到的设备

	if( FAILED(hr) )
	{
		// 尝试使用16位深度缓存
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = m_pD3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			m_HWnd,
			m_Vp,
			&d3dpp,
			&m_pDevice);

		if( FAILED(hr) )
		{
			m_pD3d9->Release(); // 创建失败,释放d3d9对象
			::MessageBox(0, TEXT("D3D创建失败"), 0, 0);
			return false;
		}
	}

	//m_pD3d9->Release();
	SetupCamera();
	SetupMesh();
	return true;
}

D3DLIGHT9 RenderSystem::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	
	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Direction = *direction;
	
	return light;
}

D3DLIGHT9 RenderSystem::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	
	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	
	return light;
}

D3DLIGHT9 RenderSystem::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	
	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;
	
	return light;
}

D3DMATERIAL9 RenderSystem::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

void RenderSystem::SetFont(int fontheight,int fontwidth,int fontweight,LPCSTR fontstyle)
{
	D3DXFONT_DESC Desc;
	
	Desc.Height = fontheight;
	Desc.Width = fontwidth;
	Desc.Weight = fontweight;
	Desc.MipLevels = D3DX_DEFAULT;
	Desc.Italic = false;
	Desc.CharSet = DEFAULT_CHARSET;
	Desc.OutputPrecision = 0;
	Desc.Quality = 0;
	Desc.PitchAndFamily = 0;
	strcpy(Desc.FaceName,fontstyle);
	
	D3DXCreateFontIndirect(m_pDevice,&Desc,&m_pFont);	
}

void RenderSystem::ShowHelp(LPCSTR help)
{
	if(m_pFont)
	m_pFont->DrawText(0,help,-1,&m_Rect,DT_TOP | DT_LEFT,0xffffffff);
}

void RenderSystem::SetupCamera()
{
	m_pCamera = new Camera(Camera::AIRCRAFT);
}

void RenderSystem::SetupMesh()
{
	m_pMaterialMesh = new MaterialMesh();
}

IDirect3DDevice9* RenderSystem::getDevice()
{
	return m_pDevice;
}

HWND RenderSystem::getHWnd()
{
	return m_HWnd;
}

LPD3DXFONT RenderSystem::getFont()
{
	return m_pFont;
}

int RenderSystem::getHeight()
{
	return m_Height;
}

int RenderSystem::getWidth()
{
	return m_Width;
}

bool RenderSystem::getOff()
{
	return m_Off;
}

void RenderSystem::setOff(bool value)
{
	m_Off = value;
}

void RenderSystem::fly(float units)
{
	m_pCamera->fly(units);
}

void RenderSystem::walk(float units)
{
	m_pCamera->walk(units);
}

void RenderSystem::strafe(float units)
{
	m_pCamera->strafe(units);
}

void RenderSystem::roll(float angle)
{
	m_pCamera->roll(angle);
}

void RenderSystem::yaw(float angle)
{
	m_pCamera->yaw(angle);
}

void RenderSystem::pitch(float angle)
{
	m_pCamera->pitch(angle);
}

void RenderSystem::getViewMatrix(D3DXMATRIX* V)
{
	m_pCamera->getViewMatrix(V);
}

bool RenderSystem::LoadMesh(LPCSTR name)
{
	if(!m_pMaterialMesh->LoadMesh(name,m_pDevice))
		return false;
	if(!m_pMaterialMesh->OptimizeMesh())
		return false;
	m_pMaterialMesh->ExtractMaterials();
	return true;
}

void RenderSystem::DrawMesh()
{
	m_pMaterialMesh->DrawMesh(m_pDevice);
}
//
// 窗口处理函数
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		if( wParam == VK_F1 )
			if(!RenderSystem::getSingleton()->getOff())
				RenderSystem::getSingleton()->setOff(true);
			else
				RenderSystem::getSingleton()->setOff(false);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}