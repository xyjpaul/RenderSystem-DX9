#include "RenderSystem.h"
#include "FBXLoader.h"

#define XFILE_NAME		  TEXT("Hall.x")
#define LOADING_TEX_NAME  TEXT("colorloading.jpg")
#define FONT_STYLE		  TEXT("Times New Roman")
#define HELP_TEXT		  TEXT("����W,S,A,D����ǰ�������ƶ�,\nʹ�÷���������ӽ�.\n����F1���ر���ʾ")
#define HELP_TEXT_SHOW	  TEXT("����F1��������ʾ.")

#ifndef __IFH__
#define __IFH__

bool PreRender();
bool BeginRender(float timeDelta);
#endif