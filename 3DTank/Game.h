#pragma once

#define GAME_CFG_APP				"3DTank"
#define GAME_CFG_MODELDIR			"ModelDir"
#define GAME_CFG_MAPDIR				"MapDir"
#define GAME_CFG_HEIGHTMAPDIR		"HeightMapDir"
#define GAME_CFG_MODELTEXTUREDIR	"ModelTextureDir"
#define GAME_CFG_MAPTEXTUREDIR		"MapTextureDir"
#define GAME_CFG_MISSIONCFG			"MissionCfg"
#define GAME_CFG_SOUNDDIR			"SoundDir"
#define GAME_CFG_SOUNDENABLE		"SoundEnable"
#define GAME_CFG_TEAMCOUNT			"TeamCount"
#define GAME_CFG_TEAMTEXTUREDIR		"TeamTextureDir"
#define GAME_CFG_TEAMTEXTUREFILE	"TeamTextureFile"
#define GAME_CFG_ENEMYHP			"EnemyHP"
#define GAME_CFG_PLAYERHP			"PlayerHP"
#define GAME_CFG_ENEMYHIT			"EnemyHit"
#define GAME_CFG_PLAYERHIT			"PlayerHit"
#define GAME_CFG_SPEED				"Speed"
#define GAME_CFG_FONT				"Font"
#define GAME_CFG_HP					"HP"
#define GAME_CFG_FPS				"FPS"
#define GAME_CFG_ENEMYNUM			"EnemyNum"
#define GAME_CFG_LOADING			"Loading"
#define GAME_CFG_SMARTAI			"SmartAI"

#define GAME_MISSION_APP			"Mission"
#define GAME_MISSION_LEVEL			"level"
#define GAME_MISSION_FINISHMISSION	"FinishMission"
#define GAME_MISSION_DEAD			"Dead"
#define GAME_MISSION_WIN			"Win"


#define GAME_MISSION_MAP			"mission%ld_map"
#define GAME_MISSION_NOTE			"mission%ld_note"
#define GAME_MISSION_ENEMYCOUNT		"EnemyNum%ld"

#define GAME_G			0.15f
#define GAME_SOUND_S	0.2f

struct VERTEX_2DPIONT
{
	D3DXVECTOR4		p;
	D3DCOLOR		c;
	D3DXVECTOR2		t;
};
#define D3DFVF_VERTEX_2DPIONT (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class CGame
{
private:
	LPDIRECT3DDEVICE9				m_pd3dDevice;
	HWND							m_hWnd;
	LPDIRECT3DVERTEXBUFFER9			m_p2DPointVB;
	LPDIRECT3DINDEXBUFFER9			m_p2DPointIB;
	UTIL_ARRAY<IDirect3DTexture9>	m_TeamTexture;
	BYTE							*m_TeamList;
	CDSoundInstance					*m_pdSound;
	CMap							*m_pHeightMap;
	C3DTankModel					*m_Player, *m_Enemy, *m_Bomb, *m_Bombing;
	CTankControl					*m_pPlayerControl;
	CSoundMannager					*m_TankSounds;
	UTIL_ARRAY<CTankControl>		m_EnemyControl;
	UTIL_ARRAY<CBombControl>		m_BombControl;
	CSoundMannager*					m_BombSounds;
	DWORD							m_dwLevelCount, m_dwLevel;
	DWORD							*m_dwEnemyCount, m_dwEnemyNum;
	DWORD							m_dwEnemyHP, m_dwPlayerHP, m_dwEnemyHit, m_dwPlayerHit;
	BOOL							m_bSoundEnable;
	DWORD							m_dwTeamCount;
	DWORD							*m_pMapMark;
	LPD3DXFONT						m_pFont;
	float							m_fFPS;
	float							m_fSpeed;
	bool							m_bLoadedModel;
	bool							m_bSmartAI;
	
	float							m_dwDot;

private:
	volatile HANDLE					m_hLoad;
	CRITICAL_SECTION				m_CS;//¡ŸΩÁ±£ª§
private:
	//config
	char m_sModelDir[256], m_sMapDir[256], m_sHeightMapDir[256],
		 m_sModelTextureDir[256], m_sMapTextureDir[256], m_sMissionCfg[256], 
		 m_sSoundDir[256], m_sTeamTextureDir[256], m_sTeamTextureFile[256],
		 m_sFont[256], m_sHP[256], m_sFPS[256], m_sEnemyNum[256], m_sLoading[256];
	//mission
	char				m_sLevel[256], m_sFinishMission[256],
						m_sDead[256], m_sWin[256];
	UTIL_ARRAY<char>	m_MissionMap, m_MissionNote;
public:
	CGame( LPDIRECT3DDEVICE9 pd3dDevice, HWND hWnd );
	~CGame(void);
	bool Create( LPSTR sConfigFilePath );
	void Release();
	HRESULT FrameMove( float fMoveTime );
	HRESULT Render( int iWidth, int iHeight );
	bool PlayerMoveTo( BYTE bLookAt );
	bool PlayerLaunchBomb();
	CTankControl* GetPlayerControl(){ return m_pPlayerControl; };
	LPD3DXFONT GetFont(){ return m_pFont; };
	float GetFPS(){ return m_fFPS; };
	char* GetFPSStr(){ return m_sFPS; };
	void ReleaseVBnIB();
	void RestoreVBnIB();
	bool IsLoading(){ return !!m_hLoad; };
	HANDLE GetThread(){ return m_hLoad; };
	DWORD GetLevel(){ return m_dwLevel; };
	DWORD GetLevelCount(){ return m_dwLevelCount; };
	void SetLevel( DWORD dwLevel ){ m_dwLevel = dwLevel; };
private:
	static UINT WINAPI LoadLevel( void *pThis );
	void ReleaseLevel();
	static UINT WINAPI LoadModel( void *pThis);
	bool IsLoadSuccess();
	void Render2DMap();
};
