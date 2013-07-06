#include "BaseMesh.h"

BaseMesh::BaseMesh()
{
	m_NumMtrls = 0;
	m_pAdjBuffer = 0;
	m_pMesh = 0;
	m_pMtrlBuffer = 0;
}

BaseMesh::~BaseMesh()
{
	if(m_pAdjBuffer){m_pAdjBuffer->Release();m_pAdjBuffer = NULL;}
	if(m_pMtrlBuffer){m_pMtrlBuffer->Release();m_pMtrlBuffer = NULL;}
	if(m_pMesh){m_pMesh->Release();m_pMesh = NULL;}
}

bool BaseMesh::LoadMesh(LPCSTR name,IDirect3DDevice9 *Device)
{
	if(!Device)
	{
		::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
		return false;
	}

	HRESULT hr = 0;
	hr = D3DXLoadMeshFromX(  
		name,
		D3DXMESH_MANAGED,
		Device,
		&m_pAdjBuffer,
		&m_pMtrlBuffer,
		0,
		&m_NumMtrls,
		&m_pMesh);
	
	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
		return false;
	}
	return true;
}

bool BaseMesh::OptimizeMesh()
{
	HRESULT hr = 0;
	hr = m_pMesh->OptimizeInplace(		
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT  |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)m_pAdjBuffer->GetBufferPointer(),
		0, 0, 0);
	
	if(FAILED(hr))
	{
		::MessageBox(0, "OptimizeInplace() - FAILED", 0, 0);
		return false;
	}
	
	return true;
}

void BaseMesh::DrawMesh()
{
	for(int i = 0; i < m_NumMtrls; i++)
	{	
		m_pMesh->DrawSubset(i);
	}	
}

ID3DXBuffer* BaseMesh::getMtrlBuffer()
{
	return m_pMtrlBuffer;
}

DWORD BaseMesh::getNumMtrls()
{
	return m_NumMtrls;
}

ID3DXMesh* BaseMesh::getMesh()
{
	return m_pMesh;
}