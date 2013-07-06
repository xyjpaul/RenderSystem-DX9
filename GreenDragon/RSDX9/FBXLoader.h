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
	// ��ʼ��FBX
	//
	bool    Initialize();
	//
	// �ӵ�ǰ·����ȡFBX�ļ�
	//
	bool    LoadFile(const char* pFbxFileName);
	//
	// ��ȡ֧�ֵ��ļ�
	//
	bool    LoadFile();
	//
	// �õ�������Ϣ��������Ϣ
	//
	void    GetFbxVerticesInfo(void** ppVBuffer);
	void    GetFbxIndicesInfo(unsigned short** ppIBuffer);
	//
	// �õ��ļ���
	//
	void	GetFileName(char* pFileName);
	//
	// ����FBX�ļ�
	//
	void    FbxObjectContent();
	//
	// �õ������������������
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
