#include"stdafx.h"
#include"Util.h"
//#include "..\myd3d\DXUtil.h"
#include"mydsound.h"
CWave::CWave( HWND hWnd )
{	
	m_hWnd = hWnd ;
	size = 0;
	pAudio = NULL;
}
CWave::~CWave()
{
	if( pAudio || hData )
		Release();
}
void CWave::Create( LPSTR lpPath )
{
	hmmio = mmioOpen(lpPath, NULL, MMIO_ALLOCBUF|MMIO_READ );//���ļ�
	if(hmmio == NULL)//�ж��ļ��Ƿ����
	{
		char strs[256];
		sprintf(strs,"Failed on opening sound: \"%s\"!",lpPath);
		MessageBox( m_hWnd, strs, "Error", NULL);
		return;
	}
	ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');//�趨�ļ�����Ϊ"WAVE"
	mmresult = mmioDescend(hmmio,&ckRiff,NULL,MMIO_FINDRIFF);//��Ѱ����
	if(mmresult != MMSYSERR_NOERROR)
	{
		char strs[256];
		sprintf(strs,"Bad format of sound: \"%s\"!",lpPath);
		MessageBox( m_hWnd, strs, "Error", NULL);
	}
	ckInfo.ckid = mmioFOURCC('f','m','t',' ');//�趨��������Ϊ"fmt "
	mmresult = mmioDescend(hmmio,&ckInfo,&ckRiff,MMIO_FINDCHUNK);//��Ѱ����
	if(mmresult != MMSYSERR_NOERROR)
	{
		char strs[256];
		sprintf(strs,"Bad format of sound: \"%s\"!",lpPath);
		MessageBox( m_hWnd, strs, "Error", NULL);
	}
	if(mmioRead(hmmio,(HPSTR)&swfmt,sizeof(swfmt)) == -1) //��ȡ�ļ���ʽ
	{
		char strs[256];
		sprintf(strs,"Failed on reading file: \"%s\"!",lpPath);
		MessageBox( m_hWnd, strs, "Error", NULL);
	}
	mmresult = mmioAscend(hmmio,&ckInfo,0);//����������
	ckInfo.ckid = mmioFOURCC('d','a','t','a');  //�趨��������Ϊ"data"
	mmresult = mmioDescend(hmmio,&ckInfo,&ckRiff,MMIO_FINDCHUNK);//��Ѱ����
	if(mmresult != MMSYSERR_NOERROR)
	{
		char strs[256];
		sprintf(strs,"Bad format of sound: \"%s\"!",lpPath);
		MessageBox( m_hWnd, strs, "Error", NULL);
	}
	bytesAudio = size = ckInfo.cksize;                       //�õ�����ʵ�ʴ�С

	hData=GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,bytesAudio);//�����ڴ�
	if( !hData )
		return;
	pAudio=(LPVOID)GlobalLock(hData);

	if( !hData || !pAudio )
	{
		MessageBox( m_hWnd, "Failed on locking memory!", "Error", NULL);
		goto FALSE_END;
	}

	mmresult = mmioRead(hmmio,(HPSTR)pAudio,bytesAudio);//��ȡ��������
	if(mmresult == -1)
	{
		MessageBox( m_hWnd, "Failed on reading sound data!", "Error", NULL);
		goto FALSE_END;
	}
	mmioClose(hmmio,0);
	return;

FALSE_END:
	mmioClose(hmmio,0);
	if(pAudio)LocalUnlock(hData);
	if(hData)GlobalFree(hData);
}
void CWave::Release()
{
	if(pAudio)LocalUnlock(hData);
	if(hData)GlobalFree(hData);
	hData = NULL;
	pAudio = NULL;
}

CDSoundInstance::CDSoundInstance( HWND hWnd ,DWORD dwLevel ,DWORD dwFlags )
{

	Listener = NULL;
	//m_BackBufCount	= BackBufferCount;
	//m_WaveCount		= WaveCount;

	/*m_ppBackBuffers = new LPCDSOUNDBUFFER[ m_BackBufCount ];
	m_ppWaves		= new LPCWAVE[m_WaveCount];

	DWORD ii;
	for( ii = 0 ; ii < m_BackBufCount ;ii++)
		m_ppBackBuffers[ii] = NULL;

	for( ii = 0 ; ii < m_WaveCount ;ii++)
		m_ppWaves[ii] = NULL;*/


	DSBUFFERDESC dsdesc;			//���������ṹ

	if( DirectSoundCreate( NULL , &m_pDS , NULL ) != S_OK )
		MessageBox(NULL,"Failed on creating DirectSound object!","Error",MB_ICONERROR);
	
	m_hWnd = hWnd;

	if( FAILED(m_pDS->SetCooperativeLevel( m_hWnd,dwLevel ) ) )
		MessageBox( m_hWnd,"Failed on SetCooperativeLevel." ,"Error", NULL );

	m_Flags = dwFlags;

	memset( &dsdesc,0, sizeof(dsdesc) );		//��սṹ����
    dsdesc.dwSize        = sizeof(dsdesc);		//���������ṹ��С
    dsdesc.dwFlags       = m_Flags;
    dsdesc.dwBufferBytes = 0;
    dsdesc.lpwfxFormat   = NULL;

    if( FAILED(m_pDS->CreateSoundBuffer( &dsdesc, &pPBuf, NULL ) ) )
		MessageBox( m_hWnd, "Failed on creating main sound buffer!" , "Error", NULL);


	memset( &pwfmt,0, sizeof(pwfmt) );
    pwfmt.wFormatTag      = WAVE_FORMAT_PCM;
    pwfmt.nChannels       = 2;               //����
    pwfmt.nSamplesPerSec  = 44100;           //����Ƶ��
    pwfmt.wBitsPerSample  = 16;              //λ
    pwfmt.nBlockAlign     = pwfmt.wBitsPerSample / 8 * pwfmt.nChannels;
    pwfmt.nAvgBytesPerSec = pwfmt.nSamplesPerSec * pwfmt.nBlockAlign;

    if( FAILED( pPBuf->SetFormat(&pwfmt) ) )       //�趨���Ÿ�ʽ
		MessageBox( m_hWnd,"Failed on setting playing format!" , "Error", NULL);

	if( FAILED( pPBuf->QueryInterface( IID_IDirectSound3DListener,(VOID**)&Listener ) ) )
		MessageBox( m_hWnd,"Failed on create Listener!" , "Error", NULL);

	pPBuf->Release(); 
}
CDSoundInstance::~CDSoundInstance()
{
	SAFE_RELEASE( Listener )
	SAFE_RELEASE( m_pDS )
}





HRESULT CDSoundInstance::SetListenerPos( float x, float y, float z, DWORD dwApply )
{
	if(Listener)
		return Listener->SetPosition( x, y, z, dwApply );
	return S_FALSE;
}
HRESULT CDSoundInstance::GetListenerPos( D3DVECTOR * pvPosition )
{
	if(Listener)
		return Listener->GetPosition( pvPosition );
	return S_FALSE;
}



void CSoundMannager::SetDir( LPSTR sDir )
{
	if( !sDir )
	{
		m_sWaveDir[0] = '\0';
		return;
	}

	if( strrchr( sDir, '\\' )[0] != '\0' )
		sprintf( m_sWaveDir,"%s\\", sDir );
	else
		strcpy( m_sWaveDir, sDir );

}
LPCWAVE CSoundMannager::GetWave( DWORD index )
{
	if( 0<= index && index < m_Wave.GetCount() )
			return m_Wave.GetUnit( index );

	return NULL;
}
DWORD CSoundMannager::AddWave( LPSTR lpPath )
{
	CWave *temp = new CWave( m_hWnd );
	char psPath[256];
	sprintf( psPath, "%s%s", m_sWaveDir, lpPath );
	temp->Create( psPath );
	return m_Wave.Append( temp );
	
}
DWORD CSoundMannager::AddBuffer( DWORD WaveIndex ,DWORD dwFlags )
{
	LPCWAVE pWave = GetWave( WaveIndex );
	if( !pWave )
		return 0xffffffff;
	CDSoundBuffer *temp = new CDSoundBuffer( pWave, m_pDSInstance->GetDSound(), m_hWnd, dwFlags );
	return m_BackBuffer.Append( temp );
}
bool CSoundMannager::ResetBuffer( DWORD dwBIndex, DWORD dwWIndex, DWORD dwFlags )
{
	LPCWAVE pWave = GetWave( dwWIndex );
	if( !pWave )
		return false;
	CDSoundBuffer **temp = m_BackBuffer.GetpUnit( dwBIndex );
	if( !temp )
		return false;
	if( pWave == (*temp)->GetWave() )
		return true;
	SAFE_DELETE( *temp );
	*temp = new CDSoundBuffer( pWave, m_pDSInstance->GetDSound(), m_hWnd, dwFlags );
	return true;
}
LPCDSOUNDBUFFER CSoundMannager::GetBackBuffer(DWORD index)
{
	if( 0<= index && index < m_BackBuffer.GetCount() )
		return m_BackBuffer.GetUnit( index );

	return NULL;
}
void CSoundMannager::Release()
{
	m_Wave.Release(true);
	ReleaseBuffer();
}
void CSoundMannager::ReleaseBuffer()
{
	m_BackBuffer.Release(true);
}




CDSoundBuffer::CDSoundBuffer( LPCWAVE pWave , LPDIRECTSOUND pDS , HWND hWnd ,DWORD dwFlags )
{
	m_pDS = pDS;
	pSBuf = NULL;
	m_pWave = pWave;
	m_hWnd = hWnd;
	Buffer3D = NULL;

	DSBUFFERDESC dsdesc;			//���������ṹ

	size = pWave->GetSize();
	swfmt= *(pWave->GetFormat()); 
	memset( &dsdesc,0,sizeof(dsdesc));        //��սṹ����
    dsdesc.dwSize  = sizeof(dsdesc);          //���ýṹ��С
    dsdesc.dwFlags = dwFlags;
    dsdesc.dwBufferBytes = size;            //���û�������С
    dsdesc.lpwfxFormat   = &swfmt;          //���û�������ʽ
	if( FAILED ( m_pDS->CreateSoundBuffer( &dsdesc, &pSBuf, NULL ) ) )
		MessageBox( m_hWnd, "Failed on creating sub sound buffer!", "Error", NULL );

	if( FAILED ( pSBuf->Lock(0,size,&pAudio,&bytesAudio,NULL,NULL,NULL)))//����������
		MessageBox( m_hWnd, "Failed on locking sub sound buffer!", "Error", NULL);

	LPVOID P = memcpy( pAudio,pWave->GetAudio(),size );//��ȡ��������
	if( !P )
		MessageBox( m_hWnd, "Failed on get sound data!", "Error", NULL);

	if( FAILED ( pSBuf->Unlock(pAudio,bytesAudio,NULL,NULL) ) )//��������Ļ�����
		MessageBox( m_hWnd, "Failed on unlocking sub sound buffer!", "Error", NULL);

	if( ( dwFlags & DSBCAPS_CTRL3D ) == DSBCAPS_CTRL3D )
		if( FAILED ( pSBuf->QueryInterface( IID_IDirectSound3DBuffer, (VOID**)&Buffer3D ) ) )//����3D������
			MessageBox( m_hWnd, "Failed on creating 3D sound buffer!", "Error", NULL);	
}
CDSoundBuffer::~CDSoundBuffer()
{
	Pause(); 
	SAFE_RELEASE( pSBuf );
	SAFE_RELEASE( Buffer3D );
}
HRESULT CDSoundBuffer::SetCurrentPosition( DWORD dwNewPosition )
{
	if(pSBuf)
		return pSBuf->SetCurrentPosition( dwNewPosition );

	return S_FALSE;
}
HRESULT CDSoundBuffer::GetCurrentPosition( LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor )
{
	if(pSBuf)
		return pSBuf->GetCurrentPosition( pdwCurrentPlayCursor, pdwCurrentWriteCursor );

	return S_FALSE;
}
HRESULT CDSoundBuffer::Play( DWORD dwFlags )
{
	if(pSBuf)
		return pSBuf->Play( 0,0,dwFlags );

	return S_FALSE;
}
HRESULT CDSoundBuffer::Pause()
{
	if(pSBuf)
		return pSBuf->Stop();

	return S_FALSE;
}
HRESULT CDSoundBuffer::Stop()
{
	HRESULT result = S_OK;

	result = Pause();
	result |= SetCurrentPosition( 0 );


	return result;
}
HRESULT CDSoundBuffer::SetVolume( LONG Volume )
{
	if(pSBuf)
		return pSBuf->SetVolume( Volume );

	return S_FALSE;
}
HRESULT CDSoundBuffer::SetPan( LONG Pan )
{
	if(pSBuf)
		return pSBuf->SetPan( Pan );

	return S_FALSE;
}
HRESULT CDSoundBuffer::GetVolume( LPLONG lpVolume )
{
	if(pSBuf)
		return pSBuf->GetVolume( lpVolume );

	return S_FALSE;
}
HRESULT CDSoundBuffer::GetPan( LPLONG lpPan )
{
	if(pSBuf)
		return pSBuf->GetPan( lpPan );

	return S_FALSE;
}
HRESULT CDSoundBuffer::SetPos( float x, float y, float z, DWORD dwApply )
{
	if(Buffer3D)
		return Buffer3D->SetPosition( x, y, z, dwApply );
	return S_FALSE;
}
HRESULT CDSoundBuffer::GetPos( D3DVECTOR * pvPosition )
{
	if(Buffer3D)
		return Buffer3D->GetPosition( pvPosition );
	return S_FALSE;
}
