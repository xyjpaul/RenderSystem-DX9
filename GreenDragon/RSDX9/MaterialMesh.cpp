#include "MaterialMesh.h"

MaterialMesh::MaterialMesh()
{
	
}

MaterialMesh::~MaterialMesh()
{
	
}

void MaterialMesh::DrawMesh(IDirect3DDevice9* Device)
{
	if(!Device)
		return ;

	for(int i = 0; i < m_Mtrls.size(); i++)
	{
		Device->SetMaterial( &m_Mtrls[i] );
		getMesh()->DrawSubset(i);
	}
}

void MaterialMesh::ExtractMaterials()
{
	if( getMtrlBuffer() != 0 && getNumMtrls() != 0 )
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)getMtrlBuffer()->GetBufferPointer();
		
		for(int i = 0; i < getNumMtrls(); i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;
			
			// save the ith material
			m_Mtrls.push_back( mtrls[i].MatD3D );	
		}
	}
}