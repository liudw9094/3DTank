#include "stdafx.h"
#include"Util.h"
#include "MyDShow.h"
//#include "..\myd3d\dxutil.h"





CDShow::CDShow()
{
	pEvent = NULL;
	pMediaControl = NULL;
	pGraph = NULL;
	m_pWc = NULL;

	CoInitialize(NULL);
    
    //建立 Media COM 对象
    CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);


    pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
	pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	

}
CDShow::~CDShow()
{
	Stop();
	SAFE_RELEASE(m_pWc)
	SAFE_RELEASE(pEvent)
    SAFE_RELEASE(pMediaControl)
    SAFE_RELEASE(pGraph)
	
    CoUninitialize();
}
HRESULT CDShow::Play(LPCWSTR strFile,HWND hWnd,BOOL IsShowCur)
{

	Create(strFile,hWnd,&m_pWc);
	Play(IsShowCur);

	return S_OK;
}
HRESULT CDShow::Create( LPCWSTR strFile , HWND hWnd , IVMRWindowlessControl **ppWC )
{
	HRESULT hr1 = S_FALSE;
	if( !ppWC )
		ppWC = &m_pWc;

	if(hWnd)
	{
		hr1=InitWindowlessVMR(hWnd, pGraph, ppWC);
		if(FAILED(hr1))
			return hr1;
	}
	return pGraph->RenderFile(strFile, NULL);

}
HRESULT CDShow::Play( BOOL IsShowCur )
{
	::ShowCursor(IsShowCur);
    return pMediaControl->Run();
}
HRESULT CDShow::Stop( BOOL IsShowCur )
{
	::ShowCursor(IsShowCur);
    pMediaControl->Stop();

	return S_OK;
}
HRESULT CDShow::Pause( BOOL IsShowCur )
{
	::ShowCursor(IsShowCur);
    pMediaControl->Pause();

	return S_OK;
}
int CDShow::Repaint( HWND hWnd )
{
	PAINTSTRUCT ps; 
	HDC         hdc; 
	RECT        rcClient; 
	GetClientRect(hWnd, &rcClient); 
	hdc = BeginPaint(hWnd, &ps); 
	if (m_pWc) 
	{ 
		// Request the VMR to paint the video.
		HRESULT hr = m_pWc->RepaintVideo(hWnd, hdc);  
	} 

	EndPaint(hWnd, &ps); 

	return 0;
}
HRESULT CDShow::InitWindowlessVMR(
	HWND hwndApp,                  // Window to hold the video. 
	IGraphBuilder* pGraph,         // Pointer to the Filter Graph Manager. 
	IVMRWindowlessControl** ppWc  // Receives a pointer to the VMR.
	)
{ 
    if (!pGraph || !ppWc) return E_POINTER;
    IBaseFilter* pVmr = NULL; 
    IVMRWindowlessControl* pWc = NULL; 
    // Create the VMR. 
    HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, 
        CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr); 
    if (FAILED(hr))
    {
        return hr;
    }
    
    // Add the VMR to the filter graph.
    hr = pGraph->AddFilter(pVmr, L"Video Mixing Renderer"); 
    if (FAILED(hr)) 
    {
        pVmr->Release();
        return hr;
    }
    // Set the rendering mode.  
    IVMRFilterConfig* pConfig; 
    hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
    if (SUCCEEDED(hr)) 
    { 
        hr = pConfig->SetRenderingMode(VMRMode_Windowless); 
        pConfig->Release(); 
    }
    if (SUCCEEDED(hr))
    {
        // Set the window. 
        hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);
        if( SUCCEEDED(hr)) 
        { 
            hr = pWc->SetVideoClippingWindow(hwndApp); 
            if (SUCCEEDED(hr))
            {
                *ppWc = pWc; // Return this as an AddRef'd pointer. 
            }
            else
            {
                // An error occurred, so release the interface.
                pWc->Release();
            }
        } 
    } 
    pVmr->Release(); 
    return hr; 
} 
