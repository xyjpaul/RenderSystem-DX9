#include "BaseMesh.h"

#ifndef __MMH__
#define __MMH__

class MaterialMesh : public BaseMesh
{
private:
	std::vector<D3DMATERIAL9>        m_Mtrls;		
public:
	MaterialMesh();
	virtual	 	  ~MaterialMesh();
	virtual void  DrawMesh(IDirect3DDevice9* Device);
	void		  ExtractMaterials();
};
#endif // __MMH__