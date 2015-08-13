#include "StdAfx.h"
#include "Util.h"
#include "DMod.h"
#include "MyDSound.h"
#include "MyDShow.h"
#include "C3DTankModel.h"
#include "Camera.h"
#include "HeightMap.h"
#include "TankControl.h"
#include "BombControl.h"
#include "Game.h"
#include "Application.h"
#include "Resource.h"
DWORD					g_dwMouseX = 0;			// 鼠标的坐标
DWORD					g_dwMouseY = 0;			// 鼠标的坐标
CApplication*			g_pApp = NULL;
CApplication::CApplication( HINSTANCE hIns )
{
	m_pDShow = NULL;
	m_bWindowed = TRUE;
	m_hInstance = hIns;
	g_pApp = this;
	m_hWnd = NULL;
	m_lpD3DDevice = NULL;
	m_lpD3D9 = NULL;
	Pause(true);
	m_bFreeCamera = false;
	m_bd3dDeviceLost = false;

	m_Left = m_Top = m_Width = m_Height = 
		m_iOrigWidth = m_iOrigHeight = 0;

	m_pGame = NULL;
	m_sWinVideo[0] = L'\0';
	m_hMenu = NULL;
}
void CApplication::Release()
{
	char szIFullScreen[10];
	sprintf( szIFullScreen, "%ld", IsFullScreen() );
	WritePrivateProfileString( VIDEO_APP, VIDEO_FULLSCREEN, szIFullScreen, FILE_CONFIG );

	SAFE_DELETE(m_pGame);
	if( m_lpD3DDevice )
		if( m_lpD3DDevice->Release() >0 )
			MessageBoxEx( "Device release error: Some objects haven't been released.\n" );
	m_lpD3DDevice = NULL;
	SAFE_RELEASE( m_lpD3D9 );
	if( m_pDShow )
	{
		SAFE_RELEASE( m_pDShow->m_pWc );
		SAFE_DELETE( m_pDShow );
	}
}
HRESULT CApplication::Create()
{
	SAFE_CALL_BOOL( InitWindow() )
	SAFE_CALL_RESULT( InitD3DDevice() )
	SAFE_CALL_RESULT( InitDSound() )
	SAFE_CALL_RESULT( InitDInput() )
	return S_OK;
}
HRESULT CApplication::InitDSound()
{
	return S_OK;
}
BOOL CApplication::SetWindowRect( RECT *const rect )
{
	m_Left = rect->left; m_Top = rect->top;
	m_Width = rect->right - rect->left;
	m_Height = rect->bottom - rect->top;
	if( m_hWnd )
	{
		if( !( ::MoveWindow( m_hWnd , m_Left, m_Top, m_Width, m_Height, TRUE ) ) )
			return FALSE;
	}
	return TRUE;
}
BOOL CApplication::InitWindow( int nCmdShow, char *sWindowTitle ) 
{
	m_iOrigWidth = m_Width = GetPrivateProfileInt( VIDEO_APP, VIDEO_WIDTH, 640, FILE_CONFIG );
	m_iOrigHeight = m_Height = GetPrivateProfileInt( VIDEO_APP, VIDEO_HEIGHT, 480, FILE_CONFIG );
	m_bWindowed = !GetPrivateProfileInt( VIDEO_APP, VIDEO_FULLSCREEN, 0, FILE_CONFIG );
	char sWindowClassName [] = "3DTank_D3DWindow";

	HWND hWnd = FindWindow( sWindowClassName, NULL );	
    if (hWnd) 
    {
        // 将焦点置于最前面的子窗口
        // “| 0x00000001”用于将所有附属窗口置于前台并
        // 激活这些窗口。
        //SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));

		MessageBox( NULL , WARNING_RUNING, "Warning!" ,MB_ICONWARNING|MB_OK|MB_TASKMODAL|MB_SETFOREGROUND);
		ShowWindow( hWnd, SW_SHOW );
		SetForegroundWindow( hWnd );

        return FALSE;
    } 

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
						m_hInstance , LoadIcon( m_hInstance, MAKEINTRESOURCE( IDI_MY3DTANK ) ),
						LoadCursor( NULL, IDC_ARROW ), NULL, MAKEINTRESOURCE(IDC_MY3DTANK),
						sWindowClassName, LoadIcon( m_hInstance, MAKEINTRESOURCE( IDI_SMALL ) ) };
	if( !m_bWindowed )
		wc.lpszMenuName = NULL;

	RegisterClassEx( &wc );



	// Create the application's window.
	if( m_bWindowed )
		m_hWnd = CreateWindow( sWindowClassName, sWindowTitle, 
				WS_OVERLAPPEDWINDOW, m_Left, m_Top, m_Width, m_Height,
				GetDesktopWindow(), NULL, wc.hInstance, NULL );
	else
	{
		m_hWnd = CreateWindow( sWindowClassName, sWindowTitle, 
								WS_POPUP|WS_SYSMENU|WS_VISIBLE, 
								m_Left, m_Top, m_Width, m_Height,
								GetDesktopWindow(), NULL, wc.hInstance, NULL );
		if( !m_hMenu )
			m_hMenu = ::LoadMenu( NULL, MAKEINTRESOURCE( IDC_MY3DTANK ) );
		SetMenu( m_hWnd, NULL );
	}
	if(!m_hWnd)
		return FALSE;
	SetDisplaySize( m_hWnd, m_Width, m_Height );
	ShowWindow( m_hWnd, nCmdShow );
	
	//UpdateWindow( m_hWnd );

	//初始化鼠标位置
	SetMouseToCenter();

	{
		//初始化摄像机
		D3DXVECTOR3 vAt( 1.0f , 0.0f , 1.0f );
		m_Camera.SetLookAtV( &vAt );
		D3DXVECTOR3 vFrom(  1.0f , 25.0f, -5.0f );
		m_Camera.SetFromV( &vFrom );
		D3DXVECTOR3 vUp(  0.0f , 1.0f, 0.0f );
		m_Camera.SetUpV( &vUp );
		m_Camera.UpData();
	}

	return TRUE;

}
HRESULT CApplication::InitD3DDevice()
{
	HRESULT hr;
	m_lpD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( !m_lpD3D9 ) return S_FALSE;

	D3DCAPS9 d3dCap;
	m_lpD3D9->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCap );

	DWORD vp = 0;
	if( (d3dCap.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	{
		// 是，支持硬件顶点处理
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		// 不，只能用软件顶点处理
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	
	if(	( d3dCap.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA ) != D3DPBLENDCAPS_SRCALPHA ||
		( d3dCap.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA ) != D3DPBLENDCAPS_INVSRCALPHA ) 
	{
		MessageBox( m_hWnd, "This device doesn't support D3DPBLENDCAPS_SRCALPHA | D3DPBLENDCAPS_INVSRCALPHA " ,
					"Error", NULL );
		return S_FALSE;
	}

 
	
	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
	m_d3dpp.Windowed = m_bWindowed;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	//m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.BackBufferHeight = m_Height;
	m_d3dpp.BackBufferWidth = m_Width;
 
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	
	m_d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	m_d3dpp.hDeviceWindow = m_hWnd;
	//hr = m_lpD3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
	//							D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
	//							&m_d3dpp, &m_lpD3DDevice );
	//if( FAILED( hr ) )
	hr = m_lpD3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
								vp, &m_d3dpp, &m_lpD3DDevice );

	if( FAILED( hr ) )
		return hr;

	
	

	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_lpD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
	pBackBuffer->Release();

	return hr;

}
void CApplication::InputProcess()
{
	if( !m_bPause )
	{
		MouseProcess();
		KeyboardProcess();
		JoyPadProcess();
	}
}
void CApplication::MouseProcess()
{
	if( m_bFreeCamera )
	{
		POINT	pt;
		float	fDelta = 0.001f;	// 鼠标的灵敏度，该值越大移动越多.

		GetCursorPos( &pt );
		int dx = pt.x - g_dwMouseX;	// 鼠标的变化值
		int dy = pt.y - g_dwMouseY;	// 鼠标的变化值

		m_Camera.RotateLocalX( dy * fDelta );	// 鼠标的Y轴旋转值为 3D world的X轴旋转值
		m_Camera.RotateLocalY( dx * fDelta );	// 鼠标的X轴旋转值为 3D world的Y轴旋转值
		SetMouseToCenter();
	}
}
void CApplication::SetMouseToCenter( BOOL bShow )
{
	POINT	pt;
	ShowCursor( bShow );	// 鼠标消失.
	RECT	rc;
	GetClientRect( m_hWnd, &rc );
	pt.x = (rc.right - rc.left) / 2;
	pt.y = (rc.bottom - rc.top) / 2;
	ClientToScreen( m_hWnd, &pt );
	SetCursorPos( pt.x, pt.y );
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
}
void CApplication::KeyboardProcess()
{
	float move = m_tFrameMove * 20.0f;

	if( m_bFreeCamera )
	{
		if( GetAsyncKeyState( 'W' ) )
			m_Camera.MoveLocalZ( move );
		if( GetAsyncKeyState( 'S' ) )
			m_Camera.MoveLocalZ( -move );
		if( GetAsyncKeyState( 'D' ) )
			m_Camera.MoveLocalX( move );
		if( GetAsyncKeyState( 'A' ) )
			m_Camera.MoveLocalX( -move );

	}
	else
	{
		if( !m_pGame )
			return;
		CTankControl *control = m_pGame->GetPlayerControl();
		if( control )
		{
			if( GetAsyncKeyState( 'W' ) )
				m_pGame->PlayerMoveTo( 0 );
			if( GetAsyncKeyState( 'S' ) )
				m_pGame->PlayerMoveTo( 2 );
			if( GetAsyncKeyState( 'D' ) )
				m_pGame->PlayerMoveTo( 1 );
			if( GetAsyncKeyState( 'A' ) )
				m_pGame->PlayerMoveTo( 3 );
			if( GetAsyncKeyState( ' ' ) )
				m_pGame->PlayerLaunchBomb();
			float x = control->GetMatrix()->_41;
			float y = control->GetMatrix()->_42;
			float z = control->GetMatrix()->_43;
			D3DXVECTOR3 v( x,y+15,z-10.0f ), v1( x,y,z );
			m_Camera.SetFromV( &v );
			m_Camera.SetLookAtV( &v1 );
			m_Camera.UpData();
		}
	}

}
HRESULT CApplication::RenderD3DEnvironment()
{
	WriteLog( "RenderD3DEnvironment...\n" );
	if( m_lpD3DDevice && m_hWnd )
	{
		
		HRESULT hr;
		if( m_bd3dDeviceLost )
		{
			WriteLog( "Device lost...\n" );
			// Test the cooperative level to see if it's okay to render
			if( FAILED( hr = m_lpD3DDevice->TestCooperativeLevel() ) )
			{
				// If the device was lost, do not render until we get it back
				if( D3DERR_DEVICELOST == hr )
					return S_OK;


				// Check if the device needs to be reset.
				if( D3DERR_DEVICENOTRESET == hr )
				{
					if( FAILED( hr = ResetD3DDevice() ) )
						return hr;
				}
				return hr;
			}
			WriteLog( "Device regeted.\n" );
			m_bd3dDeviceLost = false;
		}
		if( FAILED ( FrameMove() ) )
			return S_FALSE;

		hr = Render();
		WriteLog( "Get render hr.\n" );
		if( hr == D3DERR_DEVICELOST )
		{
			m_bd3dDeviceLost = true;
			WriteLog( "Render failed: Device.Lost\n" );
		}
		else if( FAILED ( hr ) )
		{
			WriteLog( "Render failed....\n" );
			return hr;
		}
		WriteLog( "Render Sucesssed.\n" );
	}
	WriteLog( "RenderD3DEnvironment OK.\n" );
	return S_OK;
}
HRESULT CApplication::ResetD3DDevice()
{
	WriteLog( "Reset Device\n" );
	if( m_lpD3DDevice )
	{
		if( m_pGame )
			if( !m_pGame->IsLoading() )
			{
				m_pGame->ReleaseVBnIB();
				HRESULT hr;


				//Reset d3dDevice
		
				if( FAILED( hr = m_lpD3DDevice->Reset( &m_d3dpp ) ) )
					return hr;

				//Restore backbuffer
				LPDIRECT3DSURFACE9 pBackBuffer;
				m_lpD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
				pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
				pBackBuffer->Release();
				m_pGame->RestoreVBnIB();
			}

	}
	WriteLog("Reset Device OK\n" );
    return S_OK;
}
HRESULT CApplication::ResizeD3DDevice()
{
	WriteLog( "Resize d3dDevice\n" );
	if( !m_lpD3DDevice )
	{
		MessageBoxEx( "Resize d3dDevice quit: no device.\n" );
		return S_FALSE;
	}
	if( !m_bWindowed )
	{
		WriteLog( "Resize d3dDevice quit: Full screen\n" );
		Pause( false );
		return S_OK;
	}
    RECT rcWindowBounds,rcWindowClient;

    //Get window rect
    GetWindowRect( m_hWnd, &rcWindowBounds );
    GetClientRect( m_hWnd, &rcWindowClient );

	m_Width = m_d3dpp.BackBufferWidth  = rcWindowClient.right - rcWindowClient.left;
	m_Height = m_d3dpp.BackBufferHeight = rcWindowClient.bottom - rcWindowClient.top;

	Pause( true );
	HRESULT hr = ResetD3DDevice();
	if( FAILED( hr ) )
	{
		char str[1024];
		sprintf( str, " Failed in reset rendering device!\n Return code: %s ( 0x%x )\n", DXGetErrorString(hr), hr );
		WriteLog( str );
		//MessageBoxEx( str );
		return hr;
	}
	Pause( false );
	WriteLog( "Resize d3dDevice OK\n" );
	return hr;
}
HRESULT CApplication::InitObjects()
{
	m_pGame = new CGame( GetD3DDevice(), GethWnd() );
	m_pGame->Create( ".\\config.ini" );
	GetPrivateProfileStringW( LVIDEO_APP, LVIDEO_WINVIDEO, L"", m_sWinVideo, 256, LFILE_CONFIG );
	return S_OK;
}
HRESULT CApplication::FrameMove()
{
	WriteLog( "FrameMove..\n" );
	if( m_tTime == 0 )
	{
		m_tTime = usTimeCounter(false);
		m_tFrameMove = 0.0f;
	}
	else
	{
		m_tFrameMove = usTimeCounter(false) - m_tTime;
		m_tTime += m_tFrameMove;
	}
	//usTimeCounter(true);
	
	if( !m_bPause && m_pGame )
	{
		if( ( m_pGame->GetLevel() == m_pGame->GetLevelCount() + 2 ) &&
			m_sWinVideo[0] != L'\0' )
		{
			//Play win video
			Pause( true );
			PlayVideoOnce( m_sWinVideo );
			Pause( false );
			m_pGame->SetLevel( m_pGame->GetLevel() + 1 );
		}
		m_pGame->FrameMove( m_tFrameMove );
	}
	if( m_bPause && m_pGame )
		m_pGame->FrameMove( 0.0f );
	
	InputProcess();


	D3DXMATRIX*	pmatView = m_Camera.GetViewMatrix();		// 获得摄像机矩阵.
	m_lpD3DDevice->SetTransform( D3DTS_VIEW, pmatView );			// 设置摄像机矩阵

	//设置投影矩阵
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, (800.0f/600.0f), 0.1f, 5000.0f);
	m_lpD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	//m_lpD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		//光
	{
		float x = 0.0f;
		float y = 100.0f;
		float z = 0.0f ;
		D3DLIGHT9 light;
		D3DXVECTOR3 vecLightDirUnnormalized(x, y, z);
		ZeroMemory( &light, sizeof(D3DLIGHT9) );
	    light.Type        = D3DLIGHT_POINT;
	    light.Diffuse.r   = 1.0f;
	    light.Diffuse.g   = 1.0f;
	    light.Diffuse.b   = 1.0f;
	    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecLightDirUnnormalized );
	    light.Position.x   = x;
	    light.Position.y   = y;
	    light.Position.z   = z;
	    light.Range        = 1000.0f;
		light.Attenuation1 = 0.01f;

		m_lpD3DDevice->SetLight( 0, &light );
		m_lpD3DDevice->LightEnable( 0, TRUE );
		//打开灯光计算
		m_lpD3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		//打开反光计算
		m_lpD3DDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE);


	}
	WriteLog("FrameMove OK\n");
	return S_OK;
}
void CApplication::Pause( bool bPause )
{
	m_tFrameMove = m_tTime = 0.0f;
	m_bPause = bPause;
	usTimeCounter( true );
	char str[256];
	sprintf( str, "Pause: state %d\n",(int)bPause );
	WriteLog( str );
}
HRESULT CApplication::Render()
{
	
	WriteLog("Render...\n");
	m_lpD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL , D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
	//m_lpD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
	if( SUCCEEDED( m_lpD3DDevice->BeginScene() ) )
	{
		// Rendering of scene objects happens here

		if( m_pGame )
		{
			WriteLog("Render pGame\n");
			m_pGame->Render( m_Width, m_Height );

		}
		// End the scene
		WriteLog("Render End Scence...\n");
		m_lpD3DDevice->EndScene();
		WriteLog("Render End Scence ok\n");
	}
	HRESULT hr = m_lpD3DDevice->Present( NULL, NULL, NULL, NULL );


	WriteLog("Render Finished\n");
	return hr;
	

}
void CApplication::PlayVideoOnce( WCHAR *szVideoPath )
{
	if( !szVideoPath )
		return;
	if( *szVideoPath == L'\0' )
		return;
	if( m_pDShow )
		return;
	m_pDShow = new CDShow;
	m_pDShow->Play( szVideoPath, GethWnd(), FALSE );
	long evCode = 0;

	while( (!evCode)&&(!(GetAsyncKeyState(VK_ESCAPE))) )
	{
		if( !m_pDShow )
			return;
		if( !m_pDShow->m_pWc )
		{
			SAFE_DELETE(m_pDShow );
			return;
		}

		long lWidth, lHeight; 
		HRESULT hr = m_pDShow->m_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL); 
		if (SUCCEEDED(hr))
		{
			RECT rcSrc, rcDest;
			// Set the source rectangle.
			SetRect(&rcSrc, 0, 0, lWidth, lHeight); 
				    
			// Get the window client area->
			GetClientRect(GethWnd(), &rcDest); 
			// Set the destination rectangle.
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom); 
				    
			// Set the video position.
			hr = m_pDShow->m_pWc->SetVideoPosition(&rcSrc, &rcDest); 
		}

		m_pDShow->pEvent->WaitForCompletion(0, &evCode);
		DoEvents();
	};
	m_pDShow->Stop(); 
	SAFE_RELEASE(m_pDShow->m_pWc);
	SAFE_DELETE(m_pDShow );
	Sleep(100);
}

void CApplication::ToggleFullScreen()
{
	HRESULT hr = S_OK;
	m_d3dpp.BackBufferHeight = m_Height = m_iOrigHeight;
	m_d3dpp.BackBufferWidth = m_Width = m_iOrigWidth;
	m_bWindowed = !m_bWindowed;
	m_d3dpp.Windowed = m_bWindowed;

	Pause( true );
	if( m_bWindowed )
	{
		SetWindowLong( m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW );
		SetDisplaySize( m_hWnd, m_iOrigWidth, m_iOrigHeight );
		if( m_hMenu )
			SetMenu( m_hWnd, m_hMenu );
		m_hMenu = NULL;
	}
	else
	{
		SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
		
		if( !m_hMenu )
			m_hMenu = GetMenu( m_hWnd );
		SetMenu( m_hWnd, NULL );

		SetDisplaySize( m_hWnd, m_iOrigWidth, m_iOrigHeight );
		hr = ResetD3DDevice();
		if( FAILED( hr ) )
		{
			char str[1024];
			sprintf( str, " Failed in reset rendering device!\n Return code: %s ( 0x%x )\n", DXGetErrorString(hr), hr );
			MessageBoxEx( str );
			return;
		}
	}
	hr = ResizeD3DDevice();
	if( FAILED( hr ) )
	{
		char str[1024];
		sprintf( str, " Failed in reset rendering device!\n Return code: %s ( 0x%x )\n", DXGetErrorString(hr), hr );
		MessageBoxEx( str );
		return;
	}
	SetDisplaySize( m_hWnd, m_iOrigWidth, m_iOrigHeight );
	ShowWindow( m_hWnd, SW_SHOW );
	Sleep( 1000 );
	Pause( false );
}

int CApplication::Run()
{
	CCriticalSection::Init();
	Pause( false );
	InitObjects();


	HACCEL hAccelTable;
	MSG msg;

	hAccelTable = LoadAccelerators( m_hInstance, MAKEINTRESOURCE( IDC_MY3DTANK ) );
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
	while( msg.message != WM_QUIT )
	{
		
		BOOL bGotMsg;
		if( m_bPause )
		{
			bGotMsg = GetMessage( &msg, NULL, 0U, 0U );
			WriteLog( "GetMessage\n" );
		}
		else
		{
			bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
			WriteLog( "PeekMessage\n" );
		}
			
		
		if( bGotMsg )
			if ( !TranslateAccelerator(msg.hwnd, hAccelTable, &msg) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		
		if( m_bd3dDeviceLost )
		{
			WriteLog( "m_bd3dDeviceLost == true" );
			Sleep( 100 );
		}

		CCriticalSection::Lock();
		HRESULT hr = RenderD3DEnvironment();
		CCriticalSection::Unlock();

		if( FAILED( hr ) )
		{
			char str[1024];
			sprintf( str, " Render failed!\n Return code: %s ( 0x%x )\n", DXGetErrorString(hr), hr );
			WriteLog( str );
			MessageBoxEx( str );
			return hr;
			break;
		}
		
	}
	WriteLog( "WM_QUIT" );
	CCriticalSection::Release();
	return (int) msg.wParam;
}

LRESULT CALLBACK CApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int wmId, wmEvent;
    switch( msg )
    {
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch (wmId)
			{
			case IDM_NEW:
				if( g_pApp )
				{
					if( g_pApp->GetGame() )
					{
						if( !g_pApp->GetGame()->IsLoading() && !g_pApp->m_pDShow )
						{
							g_pApp->GetGame()->Release();
							g_pApp->GetGame()->Create( FILE_CONFIG );
						}
					}
				}
				break;
			case IDM_ABOUT:
				if( g_pApp->IsFullScreen() )
					break;
				DialogBox( g_pApp->GetInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), g_pApp->GethWnd(), About );
				break;
			case IDM_EXIT:
				SendMessage( g_pApp->GethWnd(), WM_CLOSE, NULL, NULL );
				break;
			case IDM_FULLSCREEN:
				g_pApp->ToggleFullScreen();
				break;
			default:
				return DefWindowProc( hWnd, msg, wParam, lParam );
			}
			break;
		case WM_CLOSE:
			if( g_pApp->IsFullScreen() )
			{
				g_pApp->Release();
				DestroyWindow( g_pApp->GethWnd() );
				PostQuitMessage(0);
				return 0;
			}
			if( MessageBox( g_pApp->GethWnd(), "Do you really want to quit?","Quit",MB_YESNO|MB_ICONWARNING ) == IDYES)
			{
				g_pApp->Release();
				DestroyWindow( g_pApp->GethWnd() );
				PostQuitMessage(0);
			}
			return 0;
        case WM_DESTROY:
			PostQuitMessage( 0 );
            return 0;
		case WM_ACTIVATE:
			if( g_pApp->IsFreeCamera() )
				g_pApp->SetMouseToCenter();
			g_pApp->Pause( false );
			break;
		case WM_KILLFOCUS:
			g_pApp->Pause( true );
			break;
		case WM_ENTERMENULOOP:
			g_pApp->Pause( true );
			break;
		case WM_EXITMENULOOP:
			if( g_pApp->IsFreeCamera() )
				g_pApp->SetMouseToCenter();
			g_pApp->Pause( false );
			break;
        case WM_PAINT:
			if( g_pApp->GetD3DDevice() && !g_pApp->IsFullScreen() && !g_pApp->m_pDShow )
				g_pApp->Render();
			ValidateRect( g_pApp->GethWnd() , NULL );
			if( g_pApp->m_pDShow )
				g_pApp->m_pDShow->Repaint( g_pApp->GethWnd() );
            return 0;
		case WM_KEYDOWN:
			if( GetAsyncKeyState( 'C' ) )
				g_pApp->SetFreeCamera( !g_pApp->IsFreeCamera() );
			if( GetAsyncKeyState( VK_ESCAPE ) && !g_pApp->m_pDShow
				&& g_pApp->IsFullScreen() )
				PostMessage( g_pApp->GethWnd(), WM_CLOSE, NULL, NULL );
			break;
		case WM_SIZE:
			switch(wParam)
			{
				case SIZE_MINIMIZED:
					g_pApp->Pause( true );
					g_pApp->m_bWindowState = 1;
					break;
				case SIZE_MAXIMIZED:
					if( g_pApp->IsPause() && g_pApp->m_bWindowState == 1 )
						g_pApp->Pause( false );
					g_pApp->ResizeD3DDevice();
					g_pApp->m_bWindowState = 2;
					break;
				case SIZE_RESTORED:
					if( g_pApp->m_bWindowState == 1 )
					{
						g_pApp->Pause( false ); // Unpause since we're no longer minimized
						g_pApp->m_bWindowState = 0;
						g_pApp->ResizeD3DDevice();
					}
					else if( g_pApp->m_bWindowState == 2 )
					{
						g_pApp->m_bWindowState = 0;
						g_pApp->ResizeD3DDevice();
					}
					else
					{
						//g_pApp->Pause( true );
						// If we're neither maximized nor minimized, the window size 
						// is changing by the user dragging the window edges.  In this 
						// case, we don't reset the device yet -- we wait until the 
						// user stops dragging, and a WM_EXITSIZEMOVE message comes.
					}
					break;
			}
			break;
		case WM_EXITSIZEMOVE:
			g_pApp->Pause( false );
			g_pApp->ResizeD3DDevice();
			break;
		case WM_NCHITTEST:
			// Prevent the user from selecting the menu in fullscreen mode
			if( g_pApp->IsFullScreen() )
				return HTCLIENT;
			break;
		default:
			break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}
INT_PTR CALLBACK CApplication::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND hWnd_Edit = GetDlgItem( hDlg, IDC_CREDIT );
			char szCredits[1024];
			LoadString( NULL, IDS_CREDIT, szCredits, sizeof( szCredits ) );
			SetWindowText( hWnd_Edit, szCredits );
			UpdateWindow( hWnd_Edit );
			return (INT_PTR)TRUE;
		}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}