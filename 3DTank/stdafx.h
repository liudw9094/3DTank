// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#pragma   warning(disable   :   4995)

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC 
#endif
// ������뽫λ������ָ��ƽ̨֮ǰ��ƽ̨��ΪĿ�꣬���޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ����ָ���� Windows Me ����߰汾��ΪĿ�ꡣ
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����

#define _CRT_SECURE_NO_WARNINGS

// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>
#include <string.h>
#include <assert.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

 
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