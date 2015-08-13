
/*
������ʽ˵����
	��ʽ��PCM
	������2
    ����Ƶ�ʣ�44100
    λ��16
*/
class CWave
{
protected:
	WAVEFORMATEX	swfmt;					//���������ṹ
	MMCKINFO		ckRiff;				//RIFF�������Ϣ 
	MMCKINFO		ckInfo;				//�ӿ���Ϣ
	MMRESULT		mmresult;				//���ؽṹ
	HMMIO			hmmio;						//�򿪵Ķ�ý���ļ�
	DWORD			size;							//ʵ�����ϴ�С
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
	LPDIRECTSOUND m_pDS;				//DSound����ָ��
	LPCWAVE	m_pWave;
	LPDIRECTSOUNDBUFFER pSBuf;			//�λ�����ָ��
	WAVEFORMATEX swfmt;					//���������ṹ
	MMCKINFO      ckRiff;				//RIFF�������Ϣ 
	MMCKINFO      ckInfo;				//�ӿ���Ϣ
	MMRESULT	  mmresult;				//���ؽṹ
	HMMIO hmmio;						//�򿪵Ķ�ý���ļ�
	DWORD size;							//ʵ�����ϴ�С
	LPDIRECTSOUND3DBUFFER   Buffer3D;	//3D������ָ��
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

	HRESULT SetVolume( LONG Volume );//����������С
	HRESULT GetVolume( LPLONG lpVolume );//�õ�������С
	HRESULT SetPan( LONG Pan );//��������
	HRESULT GetPan( LPLONG lpPan );//�õ�����

	HRESULT SetPos( float x, float y, float z, DWORD dwApply = DS3D_IMMEDIATE );
	HRESULT GetPos( D3DVECTOR * pvPosition );
	LPCWAVE GetWave(){ return m_pWave; };
	DWORD GetStatus(){ DWORD dwStatus = 0; if( pSBuf )pSBuf->GetStatus( &dwStatus ); return dwStatus; }

};

typedef CDSoundBuffer *LPCDSOUNDBUFFER,**LPLPCDSOUNDBUFFER;


class CDSoundInstance
{
protected:
	LPDIRECTSOUND m_pDS;			//DSound����ָ��
	LPDIRECTSOUNDBUFFER pPBuf;		//��������ָ��
	

	LPDIRECTSOUND3DLISTENER Listener;

	WAVEFORMATEX pwfmt;				//���������ṹ

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