#include <d3dx9.h>
#include <string>
#include <limits>
#include <vector>

#ifndef __BMH__
#define __BMH__

class BaseMesh
{
private:		
	ID3DXBuffer*  m_pAdjBuffer;
	ID3DXBuffer*  m_pMtrlBuffer;
	DWORD         m_NumMtrls;
	ID3DXMesh*    m_pMesh;
public:
	BaseMesh();
	virtual       ~BaseMesh();
	bool		  LoadMesh(LPCSTR name, IDirect3DDevice9 *Device);
	bool		  OptimizeMesh();
	virtual void  DrawMesh();	
	ID3DXBuffer*  getMtrlBuffer();
	DWORD		  getNumMtrls();
	ID3DXMesh*	  getMesh();
};

#endif // __BMH__