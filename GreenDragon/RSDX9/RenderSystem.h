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
	IDirect3DDevice9*		m_pDevice;			//D3D�豸
	D3DCAPS9		 		m_Caps;			//�豸Caps
	int						m_Vp;				//���㴦��ģʽ
	HWND					m_HWnd;			//���ھ��
	LPCSTR					m_pWindowName;		//����(������)����	
	int						m_Width;			//����(��̨����)���
	int						m_Height;		    //����(��̨����)�߶�
	
	Camera*					m_pCamera;			//���
	MaterialMesh*			m_pMaterialMesh;	//����

	LPD3DXFONT				m_pFont;			//����
	RECT					m_Rect;			//���ھ���
	bool					m_Off;				//�Ƿ���ʾ��ʾ

	static RenderSystem*	m_pRenderSystem;   //��Ⱦϵͳָ��
	void					SetupCamera();
	void					SetupMesh();
							RenderSystem();	
public:	
	static RenderSystem*	getSingleton();
	static void				deleteSingleton();
	virtual					~RenderSystem();
	bool					InitD3D(
							HINSTANCE hInstance,			// ʵ�����.					
			 				bool windowed,					// ���ڻ�(true)ȫ��(false).
							D3DDEVTYPE deviceType);			// �豸����HAL��REF
	//
	// ���պ���
	//		
	D3DLIGHT9				InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	D3DLIGHT9				InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	D3DLIGHT9				InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);		
	//
	// ���ò��ʺ���
	//		
	D3DMATERIAL9			InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
	//
	// �������庯��
	//
	void					SetFont(int fontheight,int fontwidth,int fontweight,LPCSTR fontstyle);
	//
	// ��ʾ����
	//
	void					ShowHelp(LPCSTR help);
	//
	// Getters �� Setters
	//
	IDirect3DDevice9*		getDevice();
	HWND					getHWnd();
	LPD3DXFONT				getFont();
	int						getWidth();
	int						getHeight();
	bool					getOff();
	void					setOff(bool value);
	//
	// ��װ����
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
// ���ڴ�����
//
LRESULT CALLBACK WndProc(
						 HWND hwnd,
						 UINT msg, 
						 WPARAM wParam,
						 LPARAM lParam);
//
// �����ʽ
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