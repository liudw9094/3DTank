
class CDShow
{
protected:


	IGraphBuilder *pGraph;
    IMediaControl *pMediaControl;


	HRESULT InitWindowlessVMR( 
		HWND hwndApp,                  // Window to hold the video. 
		IGraphBuilder* pGraph,         // Pointer to the Filter Graph Manager. 
		IVMRWindowlessControl** ppWc  // Receives a pointer to the VMR.
		);
public:
	IVMRWindowlessControl *m_pWc;
	IMediaEvent   *pEvent;

public:

	CDShow();
	~CDShow();
	HRESULT Create( LPCWSTR strFile ,HWND hWnd , OUT IVMRWindowlessControl **ppWC = NULL );//If hWnd is NULL ,no *pWC out.
	HRESULT Play( BOOL IsShowSur = TRUE );
	HRESULT Stop( BOOL IsShowSur = TRUE );
	HRESULT Pause( BOOL IsShowSur = TRUE );
	int Repaint( HWND hWnd );//Must used in WM_PAINT

	HRESULT Play(LPCWSTR strFile , HWND hWnd = NULL , BOOL IsShowCursorInPlaying=TRUE );

};