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

CGame::CGame( LPDIRECT3DDEVICE9 pd3dDevice, HWND hWnd )
{
	m_pd3dDevice = pd3dDevice;
	m_hWnd = hWnd;
	m_p2DPointVB = NULL;
	m_p2DPointIB = NULL;
	m_hLoad = NULL;
	m_TeamList = NULL;
	m_pdSound = NULL;
	m_pHeightMap = NULL;
	m_Player = m_Enemy = m_Bombing = m_Bomb = NULL;
	m_pPlayerControl = NULL;
	m_TankSounds = m_BombSounds = NULL;;
	m_dwLevelCount = m_dwLevel = 0;
	m_dwEnemyHP = m_dwPlayerHP = m_dwEnemyHit = m_dwPlayerHit = 0;
	m_dwEnemyCount = NULL;
	m_dwLevel = m_dwEnemyNum = 0xffffffff;
	m_sModelDir[0] = m_sMapDir[0] = m_sHeightMapDir[0] = m_sModelTextureDir[0] =
					 m_sMapTextureDir[0] = m_sMissionCfg[0] = m_sSoundDir[0] = 
					 m_sTeamTextureDir[0] =m_sTeamTextureFile[0] = m_sFont[0] = 
					 m_sHP[0] = m_sFPS[0] = m_sEnemyNum[0] = m_sLoading[0] =
					 m_sLevel[0] = m_sFinishMission[0] = m_sDead[0] = m_sWin[0] = '\0';
	m_bSoundEnable = FALSE;
	m_pMapMark = NULL;
	m_pFont = NULL;
	m_fFPS = 0.0f;
	m_bLoadedModel = false;
	m_bSmartAI = false;
	
}

CGame::~CGame(void)
{
	Release();
}
bool CGame::Create( LPSTR sCfgPath )
{
	if( m_dwEnemyCount || m_MissionMap.GetCount() || m_MissionNote.GetCount() || m_pHeightMap )
		return false;
	
	//Game config
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_MODELDIR,
							".\\model", m_sModelDir, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_MAPDIR,
							".\\map", m_sMapDir, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_HEIGHTMAPDIR,
							".\\map\\HeightMap", m_sHeightMapDir, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_MODELTEXTUREDIR,
							".\\texture\\model", m_sModelTextureDir, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_MAPTEXTUREDIR,
							".\\texture\\map", m_sMapTextureDir, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_MISSIONCFG,
							".\\mission.ini", m_sMissionCfg, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_SOUNDDIR,
							".\\sound", m_sSoundDir, 256, sCfgPath );
	m_bSoundEnable = GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_SOUNDENABLE, 1, sCfgPath );
	m_dwTeamCount = GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_TEAMCOUNT, 1, sCfgPath );
	m_dwEnemyHP = GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_ENEMYHP, 100, sCfgPath );
	m_dwPlayerHP = GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_PLAYERHP, 100, sCfgPath );
	m_dwEnemyHit = GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_ENEMYHIT, 20, sCfgPath );
	m_dwPlayerHit = GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_PLAYERHIT, 20, sCfgPath );
	m_fSpeed = (float)GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_SPEED, 5, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_FONT,
							"Arial", m_sFont, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_HP,
							"HP:", m_sHP, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_FPS,
							"FPS:", m_sFPS, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_ENEMYNUM,
							"Count of enemy(ies):", m_sEnemyNum, 256, sCfgPath );
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_LOADING,
							"Loading", m_sLoading, 256, sCfgPath );
	m_bSmartAI = !!GetPrivateProfileInt( GAME_CFG_APP, GAME_CFG_SMARTAI, 0, sCfgPath );
	char tempTextureDir[256];
	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_TEAMTEXTUREDIR,
							".\\texture\\Team", tempTextureDir, 256, sCfgPath );
	if( strrchr( tempTextureDir, '\\' )[0] != '\0' )
		sprintf( m_sTeamTextureDir,"%s\\", tempTextureDir );
	else
		strcpy( m_sTeamTextureDir, tempTextureDir );

	GetPrivateProfileString( GAME_CFG_APP, GAME_CFG_TEAMTEXTUREFILE,
							"%d.tga", m_sTeamTextureFile, 256, sCfgPath );

	//Mission config
	m_dwLevelCount = GetPrivateProfileInt( GAME_MISSION_APP, GAME_MISSION_LEVEL, 1, m_sMissionCfg );
	GetPrivateProfileString( GAME_MISSION_APP, GAME_MISSION_FINISHMISSION,
							"Mission complish", m_sFinishMission, 256, m_sMissionCfg  );
	GetPrivateProfileString( GAME_MISSION_APP, GAME_MISSION_DEAD,
							"You dead", m_sDead, 256, m_sMissionCfg  );
	GetPrivateProfileString( GAME_MISSION_APP, GAME_MISSION_WIN,
							"All mission complish", m_sWin, 256, m_sMissionCfg  );
	
	
	m_dwEnemyCount = new DWORD[ m_dwLevelCount ];
	DWORD i;
	for( i = 1; i <= m_dwLevelCount; i++ )
	{
		char *sMap = NULL;
		char *sNote = NULL;

		try
		{
			sMap = new char[256];
			sNote = new char[256];
		}
		catch (bad_alloc &ba)
		{
			WriteLog( ba.what() );
			SAFE_DELETE_ARRAY( sMap );
			if( m_MissionMap.GetCount() )
				m_MissionMap.Release( true );
			SAFE_DELETE_ARRAY( sNote );
			if( m_MissionNote.GetCount() )
				m_MissionNote.Release( true );
			return false;
		}
		char sMissionMap[256], sMissionNote[256], sEnemyNum[256];
		sprintf( sMissionMap, GAME_MISSION_MAP, i );
		sprintf( sMissionNote, GAME_MISSION_NOTE, i );
		sprintf( sEnemyNum, GAME_MISSION_ENEMYCOUNT, i );
		GetPrivateProfileString( GAME_MISSION_APP, sMissionMap,
							"MapDefault.ini", sMap, 256, m_sMissionCfg  );
		GetPrivateProfileString( GAME_MISSION_APP, sMissionNote,
							"Mission", sNote, 256, m_sMissionCfg  );
		m_dwEnemyCount[ i - 1 ] = GetPrivateProfileInt( GAME_MISSION_APP, sEnemyNum, 10, m_sMissionCfg );
		m_MissionMap.Append( sMap );
		m_MissionNote.Append( sNote );
	}
	
	//Create font
	if( FAILED( D3DXCreateFont( m_pd3dDevice, 0, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
								m_sFont, &m_pFont ) ) )
		return false;

	
	m_dwLevel = 0;
	m_dwEnemyNum = 0;
	m_bLoadedModel = false;
	return true;
}
HRESULT CGame::FrameMove( float fMoveTime )
{
	
	if( fMoveTime != 0.0f )
		m_fFPS = 1.0f / fMoveTime;

	fMoveTime *= m_fSpeed / 10.0f;
	if( !m_hLoad && !m_dwEnemyNum  )
	{
		if( !m_bLoadedModel )
		{
			//Create a thread to load model
			m_dwDot = 1.0f;
			UINT ThreadID = 0;
			m_hLoad = (HANDLE)_beginthreadex( NULL, NULL, LoadModel, this, CREATE_SUSPENDED, &ThreadID );
			SetThreadPriority( m_hLoad, THREAD_PRIORITY_HIGHEST );
			ResumeThread( m_hLoad );
			return S_OK;
		}
		m_dwLevel++;
		if( m_dwLevel <= m_dwLevelCount )
		{
			//Create a thread to load level
			ReleaseLevel();
			m_dwDot = 1.0f;
			UINT ThreadID = 0;
			m_hLoad = (HANDLE)_beginthreadex( NULL, NULL, LoadLevel, this, CREATE_SUSPENDED, &ThreadID );
			SetThreadPriority( m_hLoad, THREAD_PRIORITY_HIGHEST );
			ResumeThread( m_hLoad );
			return S_OK;
		}
		else
			m_dwEnemyNum = 1;
	}
	if( m_hLoad )
	{
		if( WaitForSingleObject( m_hLoad, 0 ) == WAIT_TIMEOUT )
		{
			//add loading framemove
			m_dwDot += fMoveTime * 6.0f;
			if( m_dwDot > 5.0 || m_dwDot < 1.0 )
				m_dwDot = 1;
			WriteLog("Wait for object.\n");
			return S_OK;
		}
		if( !IsLoadSuccess() )
			MessageBoxEx( "Load Error!" );
		CloseHandle( m_hLoad );
		WriteLog("CloseHandel.\n");
		m_hLoad = NULL;
		//Sound of start mission
		if( m_bSoundEnable && m_TankSounds && m_pdSound )
		{
			//Radio
			LPCDSOUNDBUFFER buf;
			if( m_dwLevel == 1 )
			{
				m_TankSounds->ResetBuffer( 0, 6, DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
								DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS );
				buf = m_TankSounds->GetBackBuffer( 0 );
				buf->SetVolume( -1000 );//-10dB
				buf->Play();
			}
			else
			{
				m_TankSounds->ResetBuffer( 0, 7, DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
								DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS );
				buf = m_TankSounds->GetBackBuffer( 0 );
				buf->SetVolume( -1000 );//-10dB
				buf->Play();
			}
		}
	}
	if( m_dwLevel <= 0 )
	{
		return S_OK;
	}
	if( fMoveTime <= 0.0f )
	{
		return S_OK;
	}
	if( m_dwLevel == m_dwLevelCount + 1 )
	{
		//Sound of win
		if( m_bSoundEnable && m_TankSounds && m_pdSound )
		{
			//Radio
			LPCDSOUNDBUFFER buf;

			m_TankSounds->ResetBuffer( 0, 8, DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
							DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS );
			buf = m_TankSounds->GetBackBuffer( 0 );
			buf->SetVolume( -1000 );//-10dB
			buf->Play();
		}
		m_dwLevel++;
	}
	if( m_dwLevel > m_dwLevelCount )
	{
		return S_OK;
	}

	if( m_pPlayerControl )
	{
		if( m_pPlayerControl->GetHP() <= 0 )
		{
			return S_OK;
		}
		m_pPlayerControl->FrameMove( fMoveTime );
		//Player sound pos
		if( m_bSoundEnable && m_BombSounds && m_pdSound )
		{
			D3DXMATRIX *mat = m_pPlayerControl->GetMatrix();
			float x = mat->_41 * GAME_SOUND_S;
			float y = mat->_42 * GAME_SOUND_S;
			float z = mat->_43 * GAME_SOUND_S;
			m_pdSound->SetListenerPos( x, y, z );
		}
	}
	DWORD count = m_EnemyControl.GetCount();
	if( count )
	{
		D3DXMATRIX *playerMatrix = m_pPlayerControl->GetMatrix();
		DWORD i;
		for( i = 0; i < count; i++ )
			if( m_EnemyControl[i]->GetHP() > 0 )
			{
				Vector2D vTarget( playerMatrix->_41, playerMatrix->_43 );
				m_EnemyControl[i]->SetAITarget( &vTarget );
				m_EnemyControl[i]->FrameMove( fMoveTime );
			}
	}
	//炮弹发射/命中计算
	count = m_BombControl.GetCount();
	if( count )
	{
		DWORD i;
		for( i = 0; i < count; i++ )
		{
			DWORD id = m_BombControl[i]->FrameMove( fMoveTime, GAME_G );
			if( id )
			{
				//Hit player
				if( id == 1 )
				{
					if( m_pPlayerControl  )
					{
						m_pPlayerControl->SetHP( m_pPlayerControl->GetHP() - m_dwEnemyHit );
						//sound of hit
						D3DXMATRIX *mat = m_pPlayerControl->GetMatrix();
						float x = mat->_41 * GAME_SOUND_S;
						float y = mat->_42 * GAME_SOUND_S;
						float z = mat->_43 * GAME_SOUND_S;
						if( m_bSoundEnable && m_TankSounds && m_pdSound )
						{
							//Tank
							m_TankSounds->ResetBuffer( 1, 1 );
							LPCDSOUNDBUFFER buf = m_TankSounds->GetBackBuffer( 1 );
							buf->SetPos( x, y, z );
							buf->Play();
							//Radio
							m_TankSounds->ResetBuffer( 0, 3, DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
														DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS );
							buf = m_TankSounds->GetBackBuffer( 0 );
							buf->SetVolume( -1000 );//-10dB
							buf->Play();
						}
						//Player tank broken
						if( m_pPlayerControl->GetHP() <= 0 )
						{
							m_pPlayerControl->FrameMove( 0.0f );
							//Sound of broken
							if( m_bSoundEnable && m_TankSounds && m_pdSound )
							{
								//Tank
								m_TankSounds->ResetBuffer( 1, 2 );
								LPCDSOUNDBUFFER buf = m_TankSounds->GetBackBuffer( 1 );
								buf->SetPos( x, y, z );
								buf->Play();
								//Radio
								m_TankSounds->ResetBuffer( 0, 5, DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
														DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS );
								buf = m_TankSounds->GetBackBuffer( 0 );
								buf->SetVolume( -1000 );//-10dB
								buf->Play();
							}
						}
					}
				}
				else
				{
					if( i != 0 )
						continue;
					DWORD enemyCount = m_EnemyControl.GetCount();
					DWORD j;
					for( j = 0; j < enemyCount; j++ )
					{
						//Hit enemy
						if( m_EnemyControl[j]->GetID() == id &&
							m_EnemyControl[j]->GetHP() > 0 )
						{
							m_EnemyControl[j]->SetHP( m_EnemyControl[j]->GetHP() - m_dwPlayerHit );
							//sound of hit
							D3DXMATRIX *mat = m_EnemyControl[j]->GetMatrix();
							float x = mat->_41 * GAME_SOUND_S;
							float y = mat->_42 * GAME_SOUND_S;
							float z = mat->_43 * GAME_SOUND_S;
							if( m_bSoundEnable && m_TankSounds && m_pdSound )
							{
								m_TankSounds->ResetBuffer( j + 2, 1 );
								LPCDSOUNDBUFFER buf = m_TankSounds->GetBackBuffer( j + 2 );
								buf->SetPos( x, y, z );
								buf->Play();
							}
							//Enemy tank broken
							if( m_EnemyControl[j]->GetHP() <= 0 )
							{
								m_EnemyControl[j]->FrameMove( 0.0f );
								m_dwEnemyNum--;
								//sound of broken
								if( m_bSoundEnable && m_TankSounds && m_pdSound )
								{
									//Enemy tank
									m_TankSounds->ResetBuffer( j + 2, 2 );
									LPCDSOUNDBUFFER buf = m_TankSounds->GetBackBuffer( j + 2 );
									buf->SetPos( x, y, z );
									buf->Play();
									//Radio
									if( (rand()%3) == 0 )
									{
										m_TankSounds->ResetBuffer( 0, 4, DSBCAPS_STATIC | DSBCAPS_CTRLPAN |
																	DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS );
										buf = m_TankSounds->GetBackBuffer( 0 );
										buf->SetVolume( -1000 );//-10dB
										buf->Play();
									}
								}
							}
						}
					}
				}
			}
			if( i == 0 )
				continue;
			if( m_EnemyControl[ i - 1 ]->GetHP() <= 0 )
				continue;
			bool result = m_BombControl[i]->Launch();
			//sound of enemy lauch bomb
			if( m_bSoundEnable && m_BombSounds && result && m_pdSound )
			{
				m_BombSounds->ResetBuffer( i, 0 );
				LPCDSOUNDBUFFER buf = m_BombSounds->GetBackBuffer( i );
				D3DXVECTOR3 *vec = m_BombControl[i]->GetVector();
				buf->SetPos( vec->x * GAME_SOUND_S, vec->y * GAME_SOUND_S, vec->z * GAME_SOUND_S );
				buf->Play();
			}
		}
	}
	//sound of bombing
	if( m_bSoundEnable && m_BombSounds && m_pdSound )
	{
		DWORD i;
		for( i = 0; i < count; i++ )
			if( m_BombControl[i]->GetState() == 2 )
			{
				m_BombSounds->ResetBuffer( i, 1 );
				LPCDSOUNDBUFFER buf = m_BombSounds->GetBackBuffer( i );
				D3DXVECTOR3 *vec = m_BombControl[i]->GetVector();
				buf->SetPos( vec->x * GAME_SOUND_S, vec->y * GAME_SOUND_S, vec->z * GAME_SOUND_S );
				buf->Play();
			}
	}
	//sound of moving
	if( m_bSoundEnable && m_TankSounds && m_pdSound )
	{
		DWORD i;
		for( i = 0; i < count; i++ )
		{
			D3DXMATRIX *mat;
			bool bMoving;
			if( i == 0 )
			{
				mat = m_pPlayerControl->GetMatrix();
				bMoving = m_pPlayerControl->GetMovingState();
			}
			else
			{
				mat = m_EnemyControl[ i - 1 ]->GetMatrix();
				bMoving = m_EnemyControl[ i - 1 ]->GetMovingState();
			}
			if( bMoving )
			{
				LPCDSOUNDBUFFER buf = m_TankSounds->GetBackBuffer( i + 1 );
				if( buf->GetStatus() != DSBSTATUS_PLAYING )
				{
					m_TankSounds->ResetBuffer( i + 1, 0 );
					buf = m_TankSounds->GetBackBuffer( i + 1 );
					float x = mat->_41 * GAME_SOUND_S;
					float y = mat->_42 * GAME_SOUND_S;
					float z = mat->_43 * GAME_SOUND_S;
					buf->SetPos( x, y, z );
					buf->Play();
				}
			}
			else
			{
				LPCDSOUNDBUFFER buf = m_TankSounds->GetBackBuffer( i + 1 );
				if( buf->GetWave() == m_TankSounds->GetWave( 0 ) )
					buf->Pause();
			}
		}
	}

	return S_OK;
}
HRESULT CGame::Render( int iWidth, int iHeight )
{
	D3DXCOLOR FontColor;
	RECT rc = { 0, 0, iWidth, iHeight };
	char sFontStr[1024];

	if( m_hLoad )
	{
		if( WaitForSingleObject( m_hLoad, 0 ) == WAIT_TIMEOUT )
		{
			//add loading render;
			if( m_pFont  )
			{
				
				char str1[1024];
				strcpy( str1, m_sLoading );
				DWORD i;
				for( i = 0; i < (DWORD)m_dwDot; i++ )
				{
					strcat( str1, "." );
				}
				FontColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
				if( m_dwLevel > 0 && m_dwLevel <= m_dwLevelCount )
				{
					sprintf( sFontStr,"%s\n%s", m_MissionNote[ m_dwLevel - 1 ], str1 );
					if( m_dwLevel > 1 )
					{
						char sTemp[1024];
						sprintf( sTemp, "%s\n%s", m_sFinishMission, sFontStr );
						strcpy( sFontStr, sTemp );
					}
					m_pFont->DrawTextA( NULL, sFontStr, -1, &rc, DT_CENTER | DT_VCENTER, FontColor );
					return S_OK;
				}
				m_pFont->DrawTextA( NULL, str1, -1, &rc, DT_CENTER | DT_VCENTER, FontColor );
			}
			return S_OK;
		}
		return S_OK;
	}
	
	if(  m_dwLevel > m_dwLevelCount )
	{
		//Render win screen.
		if( m_pFont  )
		{
			FontColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
			m_pFont->DrawTextA( NULL, m_sWin, -1, &rc, DT_CENTER | DT_VCENTER, FontColor );
		}
		return S_OK;
	}
	if( m_pPlayerControl )
		if( m_pPlayerControl->GetHP() <= 0 )
		{
			//Render dead screen
			if( m_pFont  )
			{
				FontColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );
				m_pFont->DrawTextA( NULL, m_sDead, -1, &rc, DT_CENTER | DT_VCENTER, FontColor );
			}
			return S_OK;
		}
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );//打开灯光计算
	m_pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_PHONG);//设置光照模式为补色着色
	m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE );//打开反光计算
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );//Z缓冲（深度缓冲）：打开
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	
	//打开ALPHA计算
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); 

	//设置材质属性
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//打开材质ALPHA计算
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );//设置材质缩小过滤为线性
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );//设置材质放大过滤为线性

	//环境光亮度（为世界提供一个基本的照明亮度。没有方向、没有衰减，也不会产生阴影）
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_XRGB(177,177,177)  );

	D3DXMATRIX MatrixA(	1.0f,0,0,0,
						0,1.0f,0,0,
						0,0,1.0f,0,
						0,0,0,1.0f);
	D3DXMATRIX MatrixB(	0.015f,0,0,0,
						0,0.015f,0,0,
						0,0,0.015f,0,
						0,0,0,1.0f);
	D3DXMATRIX MatrixC(	0.015f,0,0,0,
						0,0.015f,0,0,
						0,0,0.015f,0,
						0,0,0,1.0f);


	if( m_pHeightMap )
	{
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW  );
		
		m_pd3dDevice->SetTransform( D3DTS_WORLD , &MatrixA );
		m_pHeightMap->Render();
	}
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE  );

	
	m_pd3dDevice->SetTransform( D3DTS_WORLD , &MatrixB );
	
	if( m_pPlayerControl )
		if( m_pPlayerControl->GetHP() > 0 )
			m_pPlayerControl->Render();
	
	m_pd3dDevice->SetTransform( D3DTS_WORLD , &MatrixC );
	DWORD count = m_EnemyControl.GetCount();
	if( count )
	{
		DWORD i;
		for( i = 0; i < count; i++ )
			if( m_EnemyControl[i]->GetHP() > 0 )
				m_EnemyControl[i]->Render();
	}

	//为加速关闭灯光和反光计算
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,FALSE);
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	count = m_BombControl.GetCount();
	if( count )
	{
		DWORD i;
		for( i = 0; i < count; i++ )
			m_BombControl[i]->Render();
	}
	//渲染2D地图(雷达),关闭深度缓冲.
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	Render2DMap();
	//显示PlayerHP和敌方坦克数量
	if( m_pFont && m_pPlayerControl && count )
	{
		FontColor = D3DCOLOR_ARGB( 177, 255, 0, 0 );
		sprintf( sFontStr, "%s%ld, %s%ld",
				m_sHP, m_pPlayerControl->GetHP(),
				m_sEnemyNum, m_dwEnemyNum );
		m_pFont->DrawTextA( NULL, sFontStr, -1, &rc, DT_BOTTOM | DT_LEFT, FontColor );
	}
	//Render FPS
	if( m_pFont )
	{
		char sFPS[256];
		sprintf( sFPS, "%s%f", GetFPSStr(), GetFPS() );
		m_pFont->DrawTextA( NULL, sFPS, -1, &rc, DT_TOP | DT_CENTER,
									D3DXCOLOR( 1.0f, 0.0f, 1.0f, 0.5f ) );
	}
	return S_OK;
}
void CGame::Render2DMap()
{
	if( m_pHeightMap )
	{
		m_pHeightMap->Render2D(); 
		if( m_p2DPointVB && m_pPlayerControl && m_EnemyControl.GetCount() )
		{

			void *temp;
			
			DWORD dwHeight = m_pHeightMap->GetHeightMap()->GetHeight();
			DWORD dwWidth =	m_pHeightMap->GetHeightMap()->GetWidth();
			D3DXMATRIX *mat;
			DWORD i;
			for( i = 0; i <= m_dwEnemyCount[ m_dwLevel - 1 ]; i++ )
			{
				m_p2DPointVB->Lock( 0, 0, &temp, 0 );
				VERTEX_2DPIONT *temp1 = (VERTEX_2DPIONT*)temp;
				if( i == 0 )
					mat = m_pPlayerControl->GetMatrix();
				else
					mat = m_EnemyControl[ i - 1 ]->GetMatrix();

				temp1[0].c = temp1[1].c = temp1[2].c = temp1[3].c = D3DCOLOR_XRGB(255,255,255);
				temp1[0].p.w = temp1[1].p.w = temp1[2].p.w = temp1[3].p.w =1.0f;
				temp1[0].p.z = temp1[1].p.z = temp1[2].p.z = temp1[3].p.z = 0.5f;

				temp1[0].p.x = mat->_41 / (float)dwWidth * MAP2D_WIDTH + 4.0f;
				temp1[0].p.y = ( (float)dwHeight - mat->_43 ) / (float)dwHeight * MAP2D_HEIGHT - 4.0f;
				temp1[1].p.x = mat->_41 / (float)dwWidth * MAP2D_WIDTH + 4.0f;
				temp1[1].p.y = ( (float)dwHeight - mat->_43 ) / (float)dwHeight * MAP2D_HEIGHT + 4.0f;
				temp1[2].p.x = mat->_41 / (float)dwWidth * MAP2D_WIDTH - 4.0f;
				temp1[2].p.y = ( (float)dwHeight - mat->_43 ) / (float)dwHeight * MAP2D_HEIGHT + 4.0f;
				temp1[3].p.x = mat->_41 / (float)dwWidth * MAP2D_WIDTH - 4.0f;
				temp1[3].p.y = ( (float)dwHeight - mat->_43 ) / (float)dwHeight * MAP2D_HEIGHT - 4.0f;
				
				temp1[0].t.x = temp1[1].t.x = temp1[1].t.y = temp1[2].t.y = 1.0f;
				temp1[3].t.x = temp1[2].t.x = temp1[0].t.y = temp1[3].t.y = 0.0f;
				
				m_p2DPointVB->Unlock();
				if( m_pPlayerControl->GetHP() > 0 && i == 0 )
				{
					m_pd3dDevice->SetFVF( D3DFVF_VERTEX_2DPIONT );
					m_pd3dDevice->SetStreamSource( 0, m_p2DPointVB, 0, sizeof( VERTEX_2DPIONT ) );
					m_pd3dDevice->SetIndices( m_p2DPointIB );
					m_pd3dDevice->SetTexture( 0, m_TeamTexture[0] );
					m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );
					continue;
				}
				if( i == 0 )
					continue;
				if( m_EnemyControl[ i - 1 ]->GetHP() > 0 )
				{
					m_pd3dDevice->SetFVF( D3DFVF_VERTEX_2DPIONT );
					m_pd3dDevice->SetStreamSource( 0, m_p2DPointVB, 0, sizeof( VERTEX_2DPIONT ) );
					m_pd3dDevice->SetIndices( m_p2DPointIB );
					m_pd3dDevice->SetTexture( 0, m_TeamTexture[1] );
					m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );
				}
			}
		}
	}
	
}
bool CGame::PlayerMoveTo( BYTE bLookAt )
{
	if( !m_pPlayerControl || m_hLoad || !m_bLoadedModel )
		return false;;
	bool result = m_pPlayerControl->SetLookAt( bLookAt%4 );
	result &= m_pPlayerControl->MoveToLookAt();
	return result;
}
bool CGame::PlayerLaunchBomb()
{
	if( m_BombControl.GetCount() && !m_hLoad)
	{
		bool result = m_BombControl[0]->Launch();
		if( m_bSoundEnable && m_BombSounds && result && m_pdSound )
		{
			m_BombSounds->ResetBuffer( 0, 0 );
			LPCDSOUNDBUFFER buf = m_BombSounds->GetBackBuffer( 0 );
			D3DXVECTOR3 *vec = m_BombControl[0]->GetVector();
			buf->SetPos( vec->x * GAME_SOUND_S, vec->y * GAME_SOUND_S, vec->z * GAME_SOUND_S );
			buf->Play();
		}
	}
	return false;
}
void CGame::ReleaseVBnIB()
{
	WriteLog( "CGame Release VB and IB\n" );
	if( m_Player )
		m_Player->ReleaseVBnIB();
	if( m_Enemy )
		m_Enemy->ReleaseVBnIB();
	if(  m_Bomb )
		 m_Bomb->ReleaseVBnIB();
	if( m_Bombing )
		m_Bombing->ReleaseVBnIB();
	if( m_pHeightMap )
		m_pHeightMap->ReleaseVBnIB();
	SAFE_RELEASE( m_p2DPointVB );
	SAFE_RELEASE( m_p2DPointIB );
	SAFE_RELEASE( m_pFont );
}
void CGame::RestoreVBnIB()
{
	WriteLog( "CGame Restore VB and IB\n" );
	if( m_Player )
		m_Player->InitVBnIB();
	if( m_Enemy )
		m_Enemy->InitVBnIB();
	if(  m_Bomb )
		 m_Bomb->InitVBnIB();
	if( m_Bombing )
		m_Bombing->InitVBnIB();
	if( m_pHeightMap )
		m_pHeightMap->InitVBnIB();

	if( !m_pFont )
		D3DXCreateFont( m_pd3dDevice, 0, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
						OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
						m_sFont, &m_pFont );
	
	if( FAILED( m_pd3dDevice->CreateVertexBuffer(
				4 * sizeof(VERTEX_2DPIONT), 0,
				D3DFVF_VERTEX_2DPIONT,D3DPOOL_DEFAULT,
				&( m_p2DPointVB ), NULL ) ) )
		return;
	if( FAILED( m_pd3dDevice->CreateIndexBuffer(
				2 * sizeof( HEIGHT_INDEX ), 0,
				D3DFMT_INDEX32,D3DPOOL_DEFAULT,
				&( m_p2DPointIB ), NULL ) ) )
		return;
	void *tempIB;
	m_p2DPointIB->Lock( 0, 0, &tempIB, 0 );
	HEIGHT_INDEX *tempIB1 = (HEIGHT_INDEX*)tempIB;

	tempIB1[0]._0 = 0;tempIB1[0]._1 = 1;tempIB1[0]._2 = 2;
	tempIB1[1]._0 = 0;tempIB1[1]._1 = 2;tempIB1[1]._2 = 3;
	
	m_p2DPointIB->Unlock();
}

bool CGame::IsLoadSuccess()
{
	DWORD code;
	if( !m_hLoad )
		return false;
	if( WaitForSingleObject( m_hLoad, 0 ) == WAIT_TIMEOUT )
		return false;
	if( !GetExitCodeThread( m_hLoad, &code ) )
		return false;
	if( code )
	{
		char msg[256];
		sprintf( msg, "Bad return code %d in loading process!", code );
		WriteLog( msg );
		#ifdef _DEBUG

			MessageBoxEx( msg );
		#endif
		return false;
	}
	return true;
}
UINT WINAPI CGame::LoadModel( void *pThis )
{
	
	WriteLog("LoadModel...\n");

	CCriticalSection::Lock();
	CGame *pthis = (CGame*)pThis;

	LPDIRECT3DDEVICE9 pd3dDevice = pthis->m_pd3dDevice;
	if( !pthis->m_hWnd )
	{
		CCriticalSection::Unlock();
		return -1;
	}
	if( pthis->m_Player || pthis->m_Enemy || pthis->m_Bomb ||
		pthis->m_Bombing )
	{
		CCriticalSection::Unlock();
		return -2;
	}
	//if( pthis->m_pFont )
	//	return -4;
	if( pthis->m_p2DPointVB || pthis->m_p2DPointIB)
	{
		CCriticalSection::Unlock();
		return -5;
	}
	if( pthis->m_bLoadedModel )
	{
		CCriticalSection::Unlock();
		return -6;
	}
	CCriticalSection::Unlock();

	//Create model
	CCriticalSection::Lock();
	pthis->m_Player = new C3DTankModel( pd3dDevice );
	pthis->m_Enemy = new C3DTankModel( pd3dDevice );
	pthis->m_Bomb = new C3DTankModel( pd3dDevice );
	pthis->m_Bombing = new C3DTankModel( pd3dDevice );
	//Set dir
	pthis->m_Player->SetDir( pthis->m_sModelDir, pthis->m_sModelTextureDir );
	pthis->m_Enemy->SetDir( pthis->m_sModelDir, pthis->m_sModelTextureDir );
	pthis->m_Bomb->SetDir( pthis->m_sModelDir, pthis->m_sModelTextureDir );
	pthis->m_Bombing->SetDir( pthis->m_sModelDir, pthis->m_sModelTextureDir );
	CCriticalSection::Unlock();

	
	//Load model
	CCriticalSection::Lock();
	C3DTankModel **model = &(pthis->m_Player);
	char szModel[][256] = { "tank2.dmod", "tank1.dmod", "Bomb.dmod", "Bombing.dmod" };
	CCriticalSection::Unlock();
	for( DWORD i = 0; i < 4; i++ )
	{
		CCriticalSection::Lock();
		if( (*model)->Create( szModel[i] ) )
			(*model)->InitVBnIB();
		CCriticalSection::Unlock();
		model++;
	}
	
	//Load Radar mark
	CCriticalSection::Lock();
	DWORD count = pthis->m_dwTeamCount;
	CCriticalSection::Unlock();
	for( DWORD i = 1; i <= count; i++ )
	{
		CCriticalSection::Lock();
		LPDIRECT3DTEXTURE9 tempTexture;
		char	tempStr[256], tempStr1[256];
		sprintf( tempStr, pthis->m_sTeamTextureFile, i );
		sprintf( tempStr1, "%s%s", pthis->m_sTeamTextureDir, tempStr );
		if( FAILED ( D3DXCreateTextureFromFile( pd3dDevice, 
										tempStr1, &tempTexture ) ) )
		{
			CCriticalSection::Unlock();
			return -6;
		}
		pthis->m_TeamTexture.Append( tempTexture );
		CCriticalSection::Unlock();
	}
	

	CCriticalSection::Lock();
	//Setup 2DPoint list for 2DMap render
	//0 for player, the other for enemies.
	if( FAILED( pd3dDevice->CreateVertexBuffer(
				4 * sizeof(VERTEX_2DPIONT), 0,
				D3DFVF_VERTEX_2DPIONT,D3DPOOL_DEFAULT,
				&( pthis->m_p2DPointVB ), NULL ) ) )
		return -7;
	if( FAILED( pd3dDevice->CreateIndexBuffer(
				2 * sizeof( HEIGHT_INDEX ), 0,
				D3DFMT_INDEX32,D3DPOOL_DEFAULT,
				&( pthis->m_p2DPointIB ), NULL ) ) )
		return -8;
	void *tempIB;
	pthis->m_p2DPointIB->Lock( 0, 0, &tempIB, 0 );
	HEIGHT_INDEX *tempIB1 = (HEIGHT_INDEX*)tempIB;

	tempIB1[0]._0 = 0;tempIB1[0]._1 = 1;tempIB1[0]._2 = 2;
	tempIB1[1]._0 = 0;tempIB1[1]._1 = 2;tempIB1[1]._2 = 3;
	
	pthis->m_p2DPointIB->Unlock();
	CCriticalSection::Unlock();

	CCriticalSection::Lock();
	pthis->m_bLoadedModel = true;
	CCriticalSection::Unlock();
	WriteLog("LoadModel ok\n");
	
	return 0;
}
UINT WINAPI CGame::LoadLevel( void *pThis )
{
	
	WriteLog("LoadLevel...\n");
	CCriticalSection::Lock();
	CGame *pthis = (CGame*)pThis;

	

	if( !pthis->m_hWnd )
		return -1;
	if( pthis->m_EnemyControl.GetCount() || pthis->m_pPlayerControl )
		return -2;
	if( pthis->m_TankSounds || pthis->m_BombSounds )
		return -3;
	if( pthis->m_dwLevel > pthis->m_dwLevelCount )
		return -4;
	if( pthis->m_pMapMark )
		return -5;

	if( pthis->m_BombControl.GetCount() )
		return -7;
	if( pthis->m_pdSound )
		return -8;
	CCriticalSection::Unlock();

	CCriticalSection::Lock();
	DWORD dwLevel = pthis->m_dwLevel;
	DWORD dwEnemyNum = pthis->m_dwEnemyNum = pthis->m_dwEnemyCount[ dwLevel -1 ];
	CCriticalSection::Unlock();

	//Setup map and mark list
	CCriticalSection::Lock();
	pthis->m_pHeightMap = new CMap( pthis->m_pd3dDevice );
	pthis->m_pHeightMap->SetDir( pthis->m_sMapDir, pthis->m_sHeightMapDir, pthis->m_sMapTextureDir );
	pthis->m_pHeightMap->Create( pthis->m_MissionMap.GetUnit( dwLevel - 1 ) );
	DWORD mapHeight = pthis->m_pHeightMap->GetHeightMap()->GetHeight();
	DWORD mapWidth = pthis->m_pHeightMap->GetHeightMap()->GetWidth();
	DWORD markCount = mapHeight * mapWidth;
	pthis->m_pMapMark = new DWORD[ markCount ];
	memset( pthis->m_pMapMark, 0, sizeof( DWORD ) * markCount );
	CCriticalSection::Unlock();

	//Setup Player control.
	CCriticalSection::Lock();
	CTankControl *temp = new CTankControl( pthis->m_pd3dDevice );
	temp->SetID( 1 );
	temp->SetTeam( 1 );
	temp->SetControl( false );
	temp->SetMap( pthis->m_pHeightMap->GetHeightMap() );
	temp->SetModel( pthis->m_Player );
	temp->SetMarkList( pthis->m_pMapMark );
	temp->SetPoint( (DWORD)( (float)mapWidth/2 ), 3 );
	temp->SetHP( pthis->m_dwPlayerHP );
	pthis->m_pPlayerControl = temp;
	CCriticalSection::Unlock();

	//Setup Enemy control.
	DWORD i;
	for( i = 0; i < dwEnemyNum; i++ )
	{
		CCriticalSection::Lock();
		temp = new CTankControl( pthis->m_pd3dDevice );
		temp->SetID( i + 2 );
		temp->SetTeam( 2 );
		temp->SetControl( true );
		temp->SetMap( pthis->m_pHeightMap->GetHeightMap() );
		temp->SetHP( pthis->m_dwEnemyHP );
		temp->SetModel( pthis->m_Enemy );
		temp->SetMarkList( pthis->m_pMapMark );
		temp->SetPoint( i%( mapWidth - 4 ) + 2, (DWORD)( mapHeight - 2 - (float)i / ( mapWidth - 4 ) ) );
		temp->SetSmartAI( pthis->m_bSmartAI );
		pthis->m_EnemyControl.Append( temp );
		CCriticalSection::Unlock();

	}

	//Setup bomb list.
	//0 for player, the other for enemies.
	for( i = 0; i<= dwEnemyNum; i++ )
	{
		CCriticalSection::Lock();
		CBombControl *tempBomb = new CBombControl( pthis->m_pd3dDevice );
		tempBomb->SetModel( pthis->m_Bomb, pthis->m_Bombing );
		if( i == 0 )
			tempBomb->SetTankControl( pthis->m_pPlayerControl );
		else
			tempBomb->SetTankControl( pthis->m_EnemyControl[ i - 1 ] );
		pthis->m_BombControl.Append( tempBomb );
		CCriticalSection::Unlock();

	}


	
	//Init DirectSound
	CCriticalSection::Lock();
	pthis->m_pdSound = new CDSoundInstance( pthis->m_hWnd );
	CCriticalSection::Unlock();


	//Load Sounds
	CCriticalSection::Lock();
	BOOL bSoundEnable = pthis->m_bSoundEnable;
	CCriticalSection::Unlock();
	if( bSoundEnable )
	{
		CCriticalSection::Lock();
		pthis->m_TankSounds = new CSoundMannager( pthis->m_pdSound, pthis->m_hWnd );
		pthis->m_BombSounds  = new CSoundMannager( pthis->m_pdSound, pthis->m_hWnd );
		
		//Set the dir of sounds
		pthis->m_TankSounds->SetDir( pthis->m_sSoundDir );
		pthis->m_BombSounds->SetDir( pthis->m_sSoundDir );
		CCriticalSection::Unlock();
		//Setup wave
		char szWaveFile[][256] = { "Move.wav", "hited.wav", "Broken.wav",
									"RadioStriked.wav", "RadioNiceShot.wav", "RadioDead.wav",
									"RadioStart.wav", "RadioFinishMission.wav", "RadioWin.wav",
									"Bomb.wav", "Bombing.wav" };
		DWORD IsLoadSoundOK = 1;
		for( i = 0; i < 9; i++ )
		{
			CCriticalSection::Lock();
			IsLoadSoundOK = pthis->m_TankSounds->AddWave( szWaveFile[i] ) && IsLoadSoundOK;
			CCriticalSection::Unlock();
		}
		for( i = 9; i < 11; i++ )
		{
			CCriticalSection::Lock();
			IsLoadSoundOK = pthis->m_BombSounds->AddWave( szWaveFile[i] ) && IsLoadSoundOK;
			CCriticalSection::Unlock();
		}
		/*
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "Move.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "hited.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "Broken.wav" ) && IsLoadSoundOK;
		
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "RadioStriked.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "RadioNiceShot.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "RadioDead.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "RadioStart.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "RadioFinishMission.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_TankSounds->AddWave( "RadioWin.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_BombSounds->AddWave( "Bomb.wav" ) && IsLoadSoundOK;
		IsLoadSoundOK = pthis->m_BombSounds->AddWave( "Bombing.wav" ) && IsLoadSoundOK;
		*/
		if( !IsLoadSoundOK )
		{
			CCriticalSection::Lock();
			pthis->m_bSoundEnable = FALSE;
			SAFE_DELETE( pthis->m_TankSounds );
			SAFE_DELETE( pthis->m_BombSounds );
			CCriticalSection::Unlock();
			return -10;
		}
		//buffer 0 for player,the other for enemies
		for( i = 0; i < dwEnemyNum + 1; i++ )
		{
			CCriticalSection::Lock();
			pthis->m_BombSounds->AddBuffer( 0 );
			CCriticalSection::Unlock();
		}
		//buffer 0 for radio, 1 for player, the other for enemies
		for( i = 0; i <= dwEnemyNum + 2; i++ )
		{
			CCriticalSection::Lock();
			pthis->m_TankSounds->AddBuffer( 0 );
			CCriticalSection::Unlock();
		}


	}
	Sleep( 1000 );
	WriteLog("LoadLevel ok\n");
	
	return 0;
}
void CGame::ReleaseLevel()
{
	WriteLog( "CGame Release level\n" );
	SAFE_DELETE( m_BombSounds );
	SAFE_DELETE( m_TankSounds );
	SAFE_DELETE_ARRAY( m_pMapMark );
	SAFE_DELETE( m_pPlayerControl );
	SAFE_DELETE( m_pHeightMap );
	m_BombControl.Release( true );
	m_EnemyControl.Release(true);
	SAFE_DELETE( m_pdSound );
	m_dwEnemyNum = 0;
}
void CGame::Release()
{
	WriteLog( "CGame Release\n" );
	if( m_hLoad )
	{
		ResumeThread( m_hLoad );
		WaitForSingleObject( m_hLoad, INFINITE );
		CloseHandle( m_hLoad );
		m_hLoad = NULL;
	}
	ReleaseVBnIB();
	ReleaseLevel();
	
	SAFE_DELETE( m_Player );
	SAFE_DELETE( m_Enemy );
	SAFE_DELETE( m_Bomb );
	SAFE_DELETE( m_Bombing );
	

	SAFE_DELETE_ARRAY( m_dwEnemyCount );
	m_MissionMap.Release( true );
	m_MissionNote.Release( true );
	m_dwLevelCount = m_dwLevel = 0;

	DWORD count = m_TeamTexture.GetCount();
	if( count )
	{
		for( DWORD i = 0; i < count; i++ )
			m_TeamTexture[i]->Release();
		m_TeamTexture.Release();
	}
	m_bLoadedModel = false;

}