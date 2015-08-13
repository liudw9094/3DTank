// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#pragma   warning(disable   :   4995)

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC 
#endif
// 如果必须将位于下面指定平台之前的平台作为目标，请修改下列定义。
// 有关不同平台对应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以指定将 Windows Me 或更高版本作为目标。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#define _CRT_SECURE_NO_WARNINGS

// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>
#include <string.h>
#include <assert.h>

// TODO: 在此处引用程序需要的其他头文件

 
#include <commctrl.h>
#include <math.h>
#include <stdio.h>
#include <fstream>
using namespace std;


#include <d3d9.h>
#include <d3dx9tex.h>
#include <d3d9types.h>
#include <DXErr.h>
#include <tchar.h>
#include <stdio.h>
#include <dxfile.h>
#include <rmxfguid.h>
#include <WindowsX.h>
#include <stdarg.h>
#include <mmsystem.h>
#include <D3DX9Math.h>
#include <windowsx.h>
#include <dshow.h>
#include<mmreg.h>
#include<dsound.h>

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
//#pragma comment(lib,"d3dx9dt.lib")
#pragma comment(lib,"d3dxof.lib")
#pragma comment(lib,"dxerr.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Strmiids.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"comctl32.lib")


#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")





#define WARNING_RUNING "You are running this game.\n\
The game can only run in one process!\n\
Please click \"OK\" to resume the game."



 

//check memory
#ifdef _DEBUG

#include<stdlib.h>
#include<crtdbg.h>

#   define DEBUG_CLIENTBLOCK new(_CLIENT_BLOCK, __FILE__, __LINE__)
#    define new DEBUG_CLIENTBLOCK

#else

#   define DEBUG_CLIENTBLOCK

#endif

 

#ifdef _DEBUG



/*
inline void* operator new(size_t dwSize, int iLine ,char* pcFile)
{
	printf("\nnew : %s, %d\n", pcFile, iLine);
	return malloc(dwSize);
}

inline void operator delete(void* pBlock,int iLine,char* pcFile)
{
	printf("\ndelete : %s, %d\n", pcFile, iLine);
	free(pBlock);
}
inline void operator delete[](void* pBlock,int iLine,char* pcFile)
{
	printf("\ndelete : %s, %d\n", pcFile, iLine);
	free(pBlock);
}



#define new new( __LINE__ , __FILE__  )
//#define delete delete( __LINE__ , __FILE__  )*/

#endif