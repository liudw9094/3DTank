#pragma once

struct Vector2D
{
	float x,y;
	Vector2D(){};
	Vector2D( float _x, float _y ){ x = _x; y = _y; };
	inline void operator =( Vector2D &Vec )
	{
		x = Vec.x;
		y = Vec.y;
	}
};
class CTankControl
{
private:
	LPDIRECT3DDEVICE9	m_pd3dDevice;
	DWORD				m_dwID;
	long				m_lHP;
	C3DTankModel		*m_pModel;
	CHeightMap			*m_pHeightMap;
	D3DXMATRIX			m_Matrix;
	Vector2D			m_MoveTo;
	BYTE				m_bTeam;
	DWORD				*m_pMark;//Check if there're other objects or nothing when move.
	bool				m_bAIControl;
	bool				m_bMoving;
	BYTE				m_LookAt;
	float				m_fSpeed;
	Vector2D			m_AITarget;
	bool				m_bSmartAI;
	DWORD				m_dwAIRange;
public:
	CTankControl( LPDIRECT3DDEVICE9 pd3dDevice );
	~CTankControl(void);
	void SetMatrix( D3DXMATRIX *pMatrix );
	void SetControl( bool bAIContorl ){ m_bAIControl = bAIContorl; };
	void SetMarkList( DWORD *pMarkList ){ m_pMark = pMarkList; };
	void SetMap( CHeightMap *pHeightMap ){ m_pHeightMap = pHeightMap; };
	void SetModel( C3DTankModel *pModel ){ m_pModel = pModel; };
	void SetTeam( BYTE bTeam ){ m_bTeam = bTeam; };
	void SetID( DWORD dwID ){ m_dwID = dwID; };
	void SetHP( long dwHP ){ m_lHP = dwHP; if( m_pModel )FrameMove(0.0f); };
	void SetSpeed( float fSpeed ){ m_fSpeed = fSpeed; };
	void SetAITarget( Vector2D * const vTarget ){ m_AITarget = *vTarget; }//x<0 or y<0 means no target.
	void SetSmartAI( bool bSmart ){ m_bSmartAI = bSmart; };
	void SetAISearchRange( DWORD dwRange ){ m_dwAIRange = dwRange; };

	D3DXMATRIX* GetMatrix(){ return &m_Matrix; };
	CHeightMap* GetMap(){ return m_pHeightMap; };
	C3DTankModel* GetModel(){ return m_pModel; };
	DWORD* GetMarkList(){ return m_pMark; };
	DWORD GetMark( DWORD index ){ return m_pMark[index]; };
	DWORD GetMark( DWORD x, DWORD y );
	long GetHP(){ return m_lHP; };
	float GetSpeed(){ return m_fSpeed; };
	BYTE GetTeam(){ return m_bTeam; };
	DWORD GetID(){ return m_dwID; };
	bool GetMovingState(){ return m_bMoving; };
	Vector2D* GetAITarget(){ return &m_AITarget; }
	bool IsAIContorl(){ return m_bAIControl; };
	bool IsSmartAIEnable(){ return m_bSmartAI; };
	DWORD SetAISearchRange(){ return m_dwAIRange; };

	bool SetPoint( DWORD x, DWORD y );
	//0:方向与Z坐标平行,旋转角度为逆时针绕Y坐标旋转90度,最大值为3.
	bool SetLookAt( BYTE cLookAt );
	BYTE GetLookAt(){ return m_LookAt; };
	//向前移动1个单位
	bool MoveToLookAt();
	void FrameMove( float fTime );
	HRESULT Render();
private:
	void AI();
	void UpDataHeight();
	bool AI_SearchPlayer();
};
