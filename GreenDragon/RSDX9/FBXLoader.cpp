#include "FBXLoader.h"
#include <malloc.h>

CFBXLoader::CFBXLoader()
{
	m_pFbxSdkManager = NULL;
	m_pFbxSdkScene = NULL;
	
	m_pVertexBuffer = NULL;
	m_pVertexIndexBuffer = NULL;

	m_ControlVertexCount = 0; 
	m_IndexCount = 0;
}

CFBXLoader::~CFBXLoader(void)
{
	list<LPD3DXMESH>::iterator it = m_D3DMeshList.begin();
	for(;it != m_D3DMeshList.end();it++)
	{
		(*it)->Release();
	}
	if(m_pVertexBuffer){delete m_pVertexBuffer;m_pVertexBuffer = NULL;}
	if(m_pVertexIndexBuffer){delete m_pVertexIndexBuffer;m_pVertexIndexBuffer = NULL;}
	
	if(m_pFbxSdkManager)  
	{  
		m_pFbxSdkManager->Destroy();
		m_pFbxSdkManager = NULL;
	}
}

bool CFBXLoader::Initialize()
{
	// Create the FBX SDK Manager, destroy the old manager at first 
	if(m_pFbxSdkManager)  
	{  
		m_pFbxSdkManager->Destroy();
		m_pFbxSdkManager = NULL;
	}

	m_pFbxSdkManager = KFbxSdkManager::Create(); 
	if(m_pFbxSdkManager == NULL)  
	{
		return false; 
	}

	// Create an IOSettings object  
	KFbxIOSettings* ios = KFbxIOSettings::Create(m_pFbxSdkManager , IOSROOT);  
	m_pFbxSdkManager->SetIOSettings(ios);  

	//Load plugins from the executable directory (optional)
	//KString lPath = KFbxGetApplicationDirectory();
	//m_pFBXSDKManager->LoadPluginsDirectory(lPath.Buffer());

	// Create the entity that hold the whole Scene  
	m_pFbxSdkScene = KFbxScene::Create(m_pFbxSdkManager , "");  
	if(m_pFbxSdkScene == NULL)  
	{
		return false; 
	}
	return true;  

}

bool CFBXLoader::LoadFile(const char* pSeneName)
{
	if(m_pFbxSdkManager == NULL)  
	{  
		return false;  
	}  

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	int lAnimStackCount;
	//char lPassword[1024];

	// Get the file version number generate by the FBX SDK.  
	KFbxSdkManager::GetFileFormatVersion(lSDKMajor , lSDKMinor , lSDKRevision);  

	// Create an importer.
	int lFileFormat = -1;
	KFbxImporter* pKFBXImporter = KFbxImporter::Create(m_pFbxSdkManager , "");  

	if (!m_pFbxSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(pSeneName, lFileFormat) )
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		lFileFormat = m_pFbxSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
	}

	// Initialize the importer by providing a filename  
	bool importStatus = pKFBXImporter->Initialize(pSeneName , -1 , m_pFbxSdkManager->GetIOSettings());  
	pKFBXImporter->GetFileVersion(lFileMajor , lFileMinor , lFileRevision);  

	if(!importStatus)  
	{  
		return false;  
	}  

	KFbxTakeInfo* lTakeInfo; 
	if (pKFBXImporter->IsFBX())
	{
		 lAnimStackCount = pKFBXImporter->GetAnimStackCount();
		 for(int i = 0; i < lAnimStackCount; i++)
		 {
			 lTakeInfo = pKFBXImporter->GetTakeInfo(i);
		 }

		 //IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
		 //IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
		 //IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
		 //IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
		 //IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
		 //IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
		 //IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}
 
	importStatus = pKFBXImporter->Import(m_pFbxSdkScene);  


	pKFBXImporter->Destroy();  

	return importStatus;  
}

void CFBXLoader::FbxObjectContent()
{
	int i;
	KFbxNode* lNode = m_pFbxSdkScene->GetRootNode();					

	if(lNode)
	{
		FbxObjectContent(lNode);
	}

}


void CFBXLoader::FbxObjectContent(KFbxNode* pNode)
{
	if(NULL == pNode)
	{
		return;
	}

	KFbxNodeAttribute::EAttributeType lAttributeType;
	int i;

	if(pNode->GetNodeAttribute() == NULL)
	{
		//return;
	}
	else
	{
		 lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());
		switch (lAttributeType)
		{
		case KFbxNodeAttribute::eMESH:      
		  GetMeshInfo(pNode);
		  break;
		default:
		  break;
		}
	}
	for(i = 0; i < pNode->GetChildCount(); i++)
	{
		FbxObjectContent(pNode->GetChild(i));
	}	
}


void CFBXLoader::GetMeshInfo(KFbxNode* pNode)
{
	 KFbxMesh* lMesh = (KFbxMesh*)pNode->GetNodeAttribute();
	 //KFbxMesh *mesh = dynamic_cast<KFbxMesh*>(nodeAtt);
    if(!lMesh->IsTriangleMesh())
    {
        KFbxGeometryConverter converter(m_pFbxSdkManager);
        // #1
        converter.TriangulateInPlace(pNode);
        lMesh = (KFbxMesh*)(pNode->GetNodeAttribute());
        // #2
        //mesh = converter.TriangulateMesh(mesh);
    }
	 GetControlsPoints(lMesh);
	
}

void CFBXLoader::GetControlsPoints(KFbxMesh* pMesh)
{
	int i,j,k;
	m_ControlVertexCount= pMesh->GetControlPointsCount();						 
	m_IndexCount = pMesh->GetPolygonVertexCount();
	
	KFbxVector4* lControlPoints = pMesh->GetControlPoints();
	KFbxVector4  temp;
	int* index = pMesh->GetPolygonVertices();

	m_pVertexBuffer = new Vertex[m_ControlVertexCount];
	m_pVertexIndexBuffer = new unsigned short[m_IndexCount];
	
	for(i = 0; i < m_ControlVertexCount; i++)
	{
		temp = lControlPoints[i];
		m_pVertexBuffer[i]._x = temp.mData[0];
		m_pVertexBuffer[i]._y = temp.mData[1];
		m_pVertexBuffer[i]._z = temp.mData[2];		
	}
	
	for(i = 0;i < m_IndexCount;i++)
	{
		m_pVertexIndexBuffer[i] =  index[i];
	}

	GetUVs(pMesh);

	LPD3DXMESH pD3DMesh = NULL;
	D3DXCreateMeshFVF(m_IndexCount,m_ControlVertexCount,D3DXMESH_MANAGED,Vertex::FVF,
		RenderSystem::getSingleton()->getDevice(),&pD3DMesh);

	Vertex* v;
	v = m_pVertexBuffer;

	
	Vertex* pVertex;
	pD3DMesh->LockVertexBuffer( 0, (void**) &pVertex );
	memcpy( pVertex, v,_msize(v));
	pD3DMesh->UnlockVertexBuffer();

	unsigned short* vindex;
	vindex = m_pVertexIndexBuffer;
	unsigned short* pIndex;


	pD3DMesh->LockIndexBuffer( 0, (void**) &pIndex );
	memcpy( pIndex, vindex,_msize(vindex));
	pD3DMesh->UnlockIndexBuffer();

	m_D3DMeshList.push_back(pD3DMesh);
}

void CFBXLoader::GetUVs(KFbxMesh* pMesh)
{
	int j,k;
	KFbxLayerElement::ELayerElementType eType = KFbxLayerElement::eUV;
	KFbxLayer* pLayer = NULL;
	KFbxLayerElement::EReferenceMode le;

	const char* uvSetsName;
	KFbxVector2 uv;
	KFbxLayerElementArrayTemplate<KFbxVector2> *getuv;
	int polygonCount = pMesh->GetPolygonCount();
	int poloygonSize;
	int indexArrayindex;
	int uvcount;
	int uvSetCount = 0;
	int textureIndex = 0;
	int ei = 0; 

	//uvcount = pMesh->GetTextureUVCount();

	for(j = 0;j < pMesh->GetLayerCount(eType);j++)
	{
		pLayer = pMesh->GetLayer(j);
		uvSetCount = pLayer->GetUVSetCount();
		if(uvSetCount > 0)
		{
			//iterate all uv channel indexed by element_texture_type
			for(textureIndex = KFbxLayerElement::eDIFFUSE_TEXTURES;textureIndex < KFbxLayerElement::eLAST_ELEMENT_TYPE;textureIndex++)
			{
				KFbxLayerElementUV *uvElement = pLayer->GetUVs(KFbxLayerElement::ELayerElementType(textureIndex));
				if(!uvElement)
					continue;
				//uvSetsName =  uvElement->GetName();
				if(true)//!CheckUVSetsNameExists(uvSetsName))
				{

				 switch(uvElement->GetMappingMode())
				{
					case KFbxLayerElement::eBY_CONTROL_POINT:
					 switch (uvElement->GetReferenceMode())
					{
						case KFbxLayerElement::eDIRECT:		
							for(int i = 0; i < m_ControlVertexCount; i++)
							{
								uv = uvElement->GetDirectArray().GetAt(i);
								m_pVertexBuffer[i]._u = uv.mData[0];
								m_pVertexBuffer[i]._v = uv.mData[1];								
							}
						break;
						case KFbxLayerElement::eINDEX_TO_DIRECT:			 				
					    break;
					    default:
						break;
					}
					break;

				case KFbxLayerElement::eBY_POLYGON_VERTEX:
				{     
				//le = uvElement->GetReferenceMode();
                switch (uvElement->GetReferenceMode())
                {
                    case KFbxLayerElement::eDIRECT:
						break;
                    case KFbxLayerElement::eINDEX_TO_DIRECT:
						for(int i = 0;i < uvElement->GetIndexArray().GetCount();i++)
						{													
							ei = uvElement->GetIndexArray().GetAt(i);
							if(ei >= m_ControlVertexCount)
							{
								continue;
							}
							uv = uvElement->GetDirectArray().GetAt(ei);
							m_pVertexBuffer[ei]._u = (float)uv.mData[0];
							m_pVertexBuffer[ei]._v = 1 - (float)uv.mData[1];								
						}
                    break;
                    default:						
					break;
				}
				}
				 break;

				}
			 }
			}
		}
	}
}

bool CFBXLoader::LoadFile()
{
	ReadMeshFromFile(RenderSystem::getSingleton()->getHWnd());
	m_pFbxSdkManager = GetKFbxSdkManager();
	m_pFbxSdkScene = GetKFbxScene();
	return true;
}

void CFBXLoader::GetFileName(char* pFileName)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ZeroMemory(pFileName, MAX_PATH);

	ofn.lStructSize     = sizeof(ofn);
	ofn.hwndOwner       = RenderSystem::getSingleton()->getHWnd();
	ofn.lpstrFile       = pFileName;
	ofn.nMaxFile        = MAX_PATH;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.nFilterIndex    = 1;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	GetOpenFileName(&ofn);  //��ʾ�ļ�ѡ����
}

void CFBXLoader::GetFbxVerticesInfo(void** v)
{
	*v = m_pVertexBuffer;
}

void CFBXLoader::GetFbxIndicesInfo(unsigned short** i)
{
	*i = m_pVertexIndexBuffer;
}

int CFBXLoader::GetControlVertexCount()
{
	return m_ControlVertexCount;
}

int CFBXLoader::GetIndexCount()
{
	return m_IndexCount;
}
