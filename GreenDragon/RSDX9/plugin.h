#ifndef _PLUGIN_
#define _PLUGIN_
#include <fbxsdk.h>
_declspec(dllimport)	void ReadMeshFromFile(HWND);
_declspec(dllimport)	void WriteMeshToFile(HWND);
_declspec(dllimport)	KFbxSdkManager	*GetKFbxSdkManager();
_declspec(dllimport)	KFbxScene	*GetKFbxScene();
#endif