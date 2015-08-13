#include "stdafx.h"
#include "util.h"

CRITICAL_SECTION CCriticalSection::m_CS;
bool CCriticalSection::m_bCSInited = false;
static char sUtil_LogPath[256] = "";

void MessageBoxEx( LPSTR str, bool err )
{
	char modulename[MAX_PATH];
	GetModuleFileName(NULL, modulename, MAX_PATH);
	if( err )
	{
		WriteLog( str  );
		printf( "%s:\n%s\n" , modulename , str  );
		#ifdef _DEBUG
			MessageBox(NULL, str, modulename, MB_ICONWARNING|MB_OK|MB_TASKMODAL|MB_SETFOREGROUND);
		#endif
		return;
	}
	printf( "%s:\n%s\n" , modulename , str  );
	MessageBox(NULL, str, modulename, MB_ICONWARNING|MB_OK|MB_TASKMODAL|MB_SETFOREGROUND);

}
float __fastcall usTimeCounter(bool   Start)
{
	static LARGE_INTEGER qwTicksPerSec;
	
    
	double   Result;   
	static   LARGE_INTEGER   t1;   
	LARGE_INTEGER   t2 = {0,0};   

	if(Start)   
	{
		QueryPerformanceFrequency( &qwTicksPerSec );
		QueryPerformanceCounter(&t1);
		return 0.0f;
	}   
	else   
	{   
		QueryPerformanceCounter(&t2);   
	}   
	double llQPFTicksPerSec = (double)qwTicksPerSec.QuadPart;
	Result   =  (double)(t2.QuadPart     -   t1.QuadPart)/llQPFTicksPerSec;   
	return (float)Result;   
}

BOOL SetDisplaySize( HWND hWnd , int width , int height )
{
	
	RECT rect ;
	HDC hdc ;

	hdc = GetDC( hWnd );
	rect.left = (GetDeviceCaps( hdc , HORZRES ) - width )/2;
	rect.top = (GetDeviceCaps( hdc , VERTRES ) - height )/2;
	ReleaseDC( hWnd , hdc );
	rect.right = rect.left + width ;
	rect.bottom = rect.top + height ;

	AdjustWindowRectEx( &rect , GetWindowLong( hWnd , GWL_STYLE ) , (GetMenu( hWnd)!=NULL) , 
		GetWindowLong( hWnd , GWL_EXSTYLE) );
	

	SetWindowPos( hWnd, HWND_NOTOPMOST,
                        rect.left, rect.top,
                        rect.right - rect.left ,
						rect.bottom - rect.top,
                        SWP_SHOWWINDOW );

	//MoveWindow( hWnd , rect.left , rect.top , rect.right - rect.left , rect.bottom - rect.top , true );

	
	return TRUE;
}
char *GetFileName( LPSTR sPath )
{
	static char szFileName[MAX_PATH] = "";   
	char *p = strrchr( sPath,'\\' );   
	if(p != NULL)   
	{
		strcpy(szFileName,(p+1));
		return szFileName;
	}
	return sPath;
}
char *GetFileTittle( LPSTR sPath )
{
	static char szFileTittle[MAX_PATH] = "";  
	char FileName[MAX_PATH];
	strcpy( FileName, GetFileName( sPath ) );
	char *p = strrchr( FileName,'.' );   
	if(p != NULL)   
	{
		*p = '\0';
		strcpy(szFileTittle,FileName);
		return szFileTittle;
	}
	return sPath;
}

void DoEvents()
{
	MSG msg;
	BOOL bGotMsg;
	bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
	if( bGotMsg )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}
void WriteLog( const char* slog )
{
	if( *sUtil_LogPath == '\0' ) 
		return;
	fstream fout( sUtil_LogPath ,ios_base::out|ios_base::app );
	fout<<slog;
	fout.close();
}
void SetLogPath( const char* sPath )
{
	if( strlen( sPath ) > 255 )
	{
		assert(false);
		return;
	}
	strcpy( sUtil_LogPath, sPath );
}
void ClearLog()
{
	if( *sUtil_LogPath == '\0' ) 
		return;
	fstream fout( sUtil_LogPath , ios_base::out );
	fout<<"";
	fout.close();
}
CCriticalSection::CCriticalSection()
{
}
CCriticalSection::~CCriticalSection()
{
}
void CCriticalSection::Init( DWORD dwSpinCount )
{
	if( !m_bCSInited )
	{
		if( dwSpinCount )
			InitializeCriticalSectionAndSpinCount( &m_CS, dwSpinCount );
		else
			InitializeCriticalSection( &m_CS );
	}
	m_bCSInited = true;
}
void CCriticalSection::Release()
{
	if( m_bCSInited )
		DeleteCriticalSection(&m_CS);
	m_bCSInited = false;
}
void CCriticalSection::Lock()
{
	EnterCriticalSection(&m_CS);
}
void CCriticalSection::Unlock()
{
		LeaveCriticalSection( &m_CS );
}
