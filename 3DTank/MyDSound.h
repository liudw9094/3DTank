
/*
声音格式说明：
	格式：PCM
	声道：2
    播放频率：44100
    位：16
*/
class CWave
{
protected:
	WAVEFORMATEX	swfmt;					//声明声音结构
	MMCKINFO		ckRiff;				//RIFF区块的信息 
	MMCKINFO		ckInfo;				//子块信息
	MMRESULT		mmresult;				//返回结构
	HMMIO			hmmio;						//打开的多媒体文件
	DWORD			size;							//实际资料大小
	LPVOID			pAudio;
	DWORD			bytesAudio;
	HWND			m_hWnd;
	HGLOBAL			hData;
public:
	CWave( HWND hWnd );
	~CWave();
	void Create( LPSTR lpPath );
	void Release();
	DWORD GetSize(){ return size; }
	DWORD GetBytes(){return bytesAudio; }
	LPVOID GetAudio(){ return pAudio; }
	LPWAVEFORMATEX GetFormat(){ return &swfmt; };	
};

typedef CWave *LPCWAVE,**LPLPCWAVE;

class CDSoundBuffer
{
protected:
	LPDIRECTSOUND m_pDS;				//DSound对象指针
	LPCWAVE	m_pWave;
	LPDIRECTSOUNDBUFFER pSBuf;			//次缓冲区指针
	WAVEFORMATEX swfmt;					//声明声音结构
	MMCKINFO      ckRiff;				//RIFF区块的信息 
	MMCKINFO      ckInfo;				//子块信息
	MMRESULT	  mmresult;				//返回结构
	HMMIO hmmio;						//打开的多媒体文件
	DWORD size;							//实际资料大小
	LPDIRECTSOUND3DBUFFER   Buffer3D;	//3D缓冲区指针
	LPVOID pAudio;
	DWORD bytesAudio;

	HWND m_hWnd;
public:
	

public:
	/*
	CDSoundBuffer(	LPSTR lpPath , LPDIRECTSOUND pDS , HWND hWnd,
					DWORD dwFlags = DSBCAPS_STATIC |
					DSBCAPS_CTRLPAN |DSBCAPS_CTRLVOLUME|
					DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRL3D	);
    */

	CDSoundBuffer( LPCWAVE pWave ,LPDIRECTSOUND pDS , HWND hWnd,
					DWORD dwFlags = DSBCAPS_STATIC |
					DSBCAPS_CTRLPAN |DSBCAPS_CTRLVOLUME|
					DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRL3D);

	~CDSoundBuffer();

	HRESULT Play( DWORD dwFlags = 0 );
	HRESULT Pause();
	HRESULT SetCurrentPosition( DWORD dwNewPosition );
	HRESULT GetCurrentPosition( LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor );
	HRESULT Stop();

	HRESULT SetVolume( LONG Volume );//设置声音大小
	HRESULT GetVolume( LPLONG lpVolume );//得到声音大小
	HRESULT SetPan( LONG Pan );//设置声道
	HRESULT GetPan( LPLONG lpPan );//得到声道

	HRESULT SetPos( float x, float y, float z, DWORD dwApply = DS3D_IMMEDIATE );
	HRESULT GetPos( D3DVECTOR * pvPosition );
	LPCWAVE GetWave(){ return m_pWave; };
	DWORD GetStatus(){ DWORD dwStatus = 0; if( pSBuf )pSBuf->GetStatus( &dwStatus ); return dwStatus; }

};

typedef CDSoundBuffer *LPCDSOUNDBUFFER,**LPLPCDSOUNDBUFFER;


class CDSoundInstance
{
protected:
	LPDIRECTSOUND m_pDS;			//DSound对象指针
	LPDIRECTSOUNDBUFFER pPBuf;		//主缓冲区指针
	

	LPDIRECTSOUND3DLISTENER Listener;

	WAVEFORMATEX pwfmt;				//声明声音结构

	HWND m_hWnd;
	DWORD m_Flags;


	//UTIL_ARRAY<CDSoundBuffer>	m_BackBuffer;
	//UTIL_ARRAY<CWave>			m_Wave;
	//DWORD m_BackBufCount;
	//DWORD m_WaveCount;
public:
	CDSoundInstance(	HWND hWnd ,DWORD dwLevel = DSSCL_PRIORITY ,
						DWORD dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D );
	~CDSoundInstance();

	LPDIRECTSOUND GetDSound(){ return m_pDS; };


	HRESULT SetListenerPos( float x, float y, float z, DWORD dwApply = DS3D_IMMEDIATE );
	HRESULT GetListenerPos( D3DVECTOR * pvPosition );


};

class CSoundMannager
{
private:
	CDSoundInstance *m_pDSInstance;
	HWND m_hWnd;
	UTIL_ARRAY<CDSoundBuffer>	m_BackBuffer;
	UTIL_ARRAY<CWave>			m_Wave;
	char						m_sWaveDir[256];
public:
	CSoundMannager( CDSoundInstance* pDSoundInstance, HWND hWnd )
	{
		m_pDSInstance = pDSoundInstance;
		m_hWnd = hWnd;
		m_sWaveDir[0] = '\0';
	};
	~CSoundMannager(){ Release(); };
	void SetDir( LPSTR sDir );
	//========================Waves=========================
	//Get a wave.
	LPCWAVE GetWave( DWORD index );
	//If success, return count, else return 0.
	DWORD AddWave( LPSTR lpPath	);
	//Get the count of waves
	DWORD GetWaverCount();




	//==========================Buffers=======================
	//Get a backbuffer.
	LPCDSOUNDBUFFER GetBackBuffer( DWORD index );
	//If success, return count, else return 0.
	DWORD AddBuffer( DWORD WaveIndex , DWORD dwFlags = 
					DSBCAPS_STATIC |DSBCAPS_CTRLPAN	|
					DSBCAPS_CTRLVOLUME|	DSBCAPS_GLOBALFOCUS|
					DSBCAPS_CTRL3D);
	bool ResetBuffer( DWORD dwBufferIndex, DWORD dwWaveIndex, 
					DWORD dwFlags = DSBCAPS_STATIC |
					DSBCAPS_CTRLPAN	| DSBCAPS_CTRLVOLUME|
					DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRL3D );
	void ReleaseBuffer();
	//Get the count of buffers.
	DWORD GetBackBufferCount();

	void Release();
};