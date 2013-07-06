#pragma once
#include <fbxsdk.h>
#include "RenderSystem.h"
#include "plugin.h"
using namespace std;

class CFBXLoader
{
public:
	        CFBXLoader();
	virtual ~CFBXLoader();
	//
	// 初始化FBX
	//
	bool    Initialize();
	//
	// 从当前路径读取FBX文件
	//
	bool    LoadFile(const char* pFbxFileName);
	//
	// 读取支持的文件
	//
	bool    LoadFile();
	//
	// 得到顶点信息，索引信息
	//
	void    GetFbxVerticesInfo(void** ppVBuffer);
	void    GetFbxIndicesInfo(unsigned short** ppIBuffer);
	//
	// 得到文件名
	//
	void	GetFileName(char* pFileName);
	//
	// 解析FBX文件
	//
	void    FbxObjectContent();
	//
	// 得到顶点个数，索引个数
	//
	int     GetControlVertexCount();
	int     GetIndexCount();
	
public:
	list<LPD3DXMESH> m_D3DMeshList;

private:	
	void	FbxObjectContent(KFbxNode* pNode);
	void	GetMeshInfo(KFbxNode* pNode);
	void	GetControlsPoints(KFbxMesh* pMesh);
	void	GetUVs(KFbxMesh* pMesh);

private:
	int				m_ControlVertexCount; 
	int				m_IndexCount;
	Vertex*			m_pVertexBuffer;
	unsigned short* m_pVertexIndexBuffer;		
	KFbxSdkManager* m_pFbxSdkManager;
	KFbxScene*		m_pFbxSdkScene;
};
