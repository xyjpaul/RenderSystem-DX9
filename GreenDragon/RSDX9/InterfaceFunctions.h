#include "RenderSystem.h"
#include "FBXLoader.h"

#define XFILE_NAME		  TEXT("Hall.x")
#define LOADING_TEX_NAME  TEXT("colorloading.jpg")
#define FONT_STYLE		  TEXT("Times New Roman")
#define HELP_TEXT		  TEXT("按下W,S,A,D进行前后左右移动,\n使用方向键调整视角.\n按下F1键关闭提示")
#define HELP_TEXT_SHOW	  TEXT("按下F1键开启提示.")

#ifndef __IFH__
#define __IFH__

bool PreRender();
bool BeginRender(float timeDelta);
#endif