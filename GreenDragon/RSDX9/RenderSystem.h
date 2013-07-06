#include <d3dx9.h>
#include <string>
#include <limits>
#include <vector>
#include <list>
#include <CommDlg.h>
#include "Camera.h"
#include "MaterialMesh.h"

#ifndef __RSH__
#define __RSH__

class RenderSystem
{
private:

	IDirect3D9*				m_pD3d9;			//DX9	
	IDirect3DDevice9*		m_pDevice;			//D3D设备
	D3DCAPS9		 		m_Caps;			//设备Caps
	int						m_Vp;				//顶点处理模式
	HWND					m_HWnd;			//窗口句柄
	LPCSTR					m_pWindowName;		//窗口(窗口类)名称	
	int						m_Width;			//窗口(后台缓存)宽度
	int						m_Height;		    //窗口(后台缓存)高度
	
	Camera*					m_pCamera;			//相机
	MaterialMesh*			m_pMaterialMesh;	//网格

	LPD3DXFONT				m_pFont;			//字体
	RECT					m_Rect;			//窗口矩形
	bool					m_Off;				//是否显示提示

	static RenderSystem*	m_pRenderSystem;   //渲染系统指针
	void					SetupCamera();
	void					SetupMesh();
							RenderSystem();	
public:	
	static RenderSystem*	getSingleton();
	static void				deleteSingleton();
	virtual					~RenderSystem();
	bool					InitD3D(
							HINSTANCE hInstance,			// 实例句柄.					
			 				bool windowed,					// 窗口化(true)全屏(false).
							D3DDEVTYPE deviceType);			// 设备类型HAL或REF
	//
	// 光照函数
	//		
	D3DLIGHT9				InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	D3DLIGHT9				InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	D3DLIGHT9				InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);		
	//
	// 设置材质函数
	//		
	D3DMATERIAL9			InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
	//
	// 设置字体函数
	//
	void					SetFont(int fontheight,int fontwidth,int fontweight,LPCSTR fontstyle);
	//
	// 显示帮助
	//
	void					ShowHelp(LPCSTR help);
	//
	// Getters 和 Setters
	//
	IDirect3DDevice9*		getDevice();
	HWND					getHWnd();
	LPD3DXFONT				getFont();
	int						getWidth();
	int						getHeight();
	bool					getOff();
	void					setOff(bool value);
	//
	// 封装函数
	//
	void					strafe(float units); 
	void					fly(float units);   
	void					walk(float units);  
	void					pitch(float angle); 
	void					yaw(float angle);  
	void					roll(float angle);
	void					getViewMatrix(D3DXMATRIX* V);
	bool					LoadMesh(LPCSTR name);
	void					DrawMesh();
};

//
// 窗口处理函数
//
LRESULT CALLBACK WndProc(
						 HWND hwnd,
						 UINT msg, 
						 WPARAM wParam,
						 LPARAM lParam);
//
// 顶点格式
//
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		float nx, float ny, float nz,
		float u, float v)
	{
		_x  = x;  _y  = y;  _z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;
	
	static const DWORD FVF;
};

#endif 