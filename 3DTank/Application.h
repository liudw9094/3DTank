#pragma once
#define FILE_CONFIG ".\\config.ini"
#define VIDEO_APP "Video"
#define LFILE_CONFIG L".\\config.ini"
#define LVIDEO_APP L"Video"
#define VIDEO_FULLSCREEN "FullScreen"
#define VIDEO_WIDTH "Width"
#define VIDEO_HEIGHT "Height"
#define LVIDEO_STARTLOGO L"StartLogo"
#define LVIDEO_WINVIDEO	L"WinVideo"

class CApplication
{
private:/*
	C3DTankModel *a;
	CMap *b;
	DWORD *c;
	CTankControl *d;
	CTankControl *e;*/
	CGame *m_pGame;
	CCamera	m_Camera;

	HINSTANCE				m_hInstance;
	LPDIRECT3D9				m_lpD3D9;
	LPDIRECT3DDEVICE9		m_lpD3DDevice;
	HWND					m_hWnd;
	D3DPRESENT_PARAMETERS	m_d3dpp;
	D3DSURFACE_DESC			m_d3dsdBackBuffer;
	float					m_tFrameMove;
	float					m_tTime;
	int						m_Left, m_Top, m_Width, m_Height, m_iOrigWidth, m_iOrigHeight;
	bool					m_bPause;

	bool				m_bFreeCamera;

	bool				m_bd3dDeviceLost;

	BOOL				m_bWindowed;
	//Video
	WCHAR				m_sWinVideo[256]; 


	HMENU				m_hMenu;
public:
	CDShow*				m_pDShow;
	BYTE				m_bWindowState;//0 is restored, 1 is maxsize, 2 is minisize.
private:
	HRESULT InitDInput(){ return S_OK; };
	HRESULT InitDSound();
	BOOL InitWindow( int nCmdShow = SW_SHOW, char *sWindowTitle = "3DTank" );
	HRESULT InitD3DDevice();
	
public:
	HINSTANCE GetInstance(){ return m_hInstance; };
	HWND GethWnd(){ return m_hWnd; };
	LPDIRECT3DDEVICE9 GetD3DDevice() { return m_lpD3DDevice; };
	CGame *GetGame(){ return m_pGame; };
public:
	CApplication( HINSTANCE hInstance = GetModuleHandle( NULL ) );
	~CApplication(void){ Release(); };
	BOOL SetWindowRect( RECT * const rect );

	HRESULT Create();
	void Release();
	HRESULT ResetD3DDevice();
	HRESULT RenderD3DEnvironment();
	HRESULT Render();
	HRESULT FrameMove();
	HRESULT ResizeD3DDevice();
	HRESULT ReleaseVBnIB();
	//HRESULT InitVBnIB();
	HRESULT InitObjects();
	HRESULT ReleaseObjects();
	void Pause( bool bPause );

	//Input
	void InputProcess();
	void MouseProcess();
	void SetMouseToCenter( BOOL bShowMouse = TRUE );
	void KeyboardProcess();
	void JoyPadProcess(){};
	
	int Run();
	
	void SetFreeCamera( bool bFree ){ m_bFreeCamera = bFree; };
	bool IsFreeCamera(){ return m_bFreeCamera; };
	bool IsPause(){ return m_bPause; };
	
	bool IsFullScreen(){ return !m_bWindowed; }
	void PlayVideoOnce( WCHAR *szVideoPath );
	
	void ToggleFullScreen();

private:
	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

extern CApplication* g_pApp;

