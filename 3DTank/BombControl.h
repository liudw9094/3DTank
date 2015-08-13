#pragma once

class CBombControl
{
private:
	LPDIRECT3DDEVICE9	m_pd3dDevice;
	C3DTankModel		*m_pBomb, *m_pBombing;
	D3DXMATRIX			m_BombMatrix;
	CTankControl		*m_pTankControl;
	BYTE				m_State;// 0 is unlaunched, 1 is being launched, 2 is bomb.
	Vector2D			m_LookAt;
	float				m_fMovingSpeed, m_fBombingSpeed;//Speed
	float				m_fBombRange;//Range
	float				m_fVG;
public:
	CBombControl( LPDIRECT3DDEVICE9 pd3dDevice );
	~CBombControl(void);
	DWORD FrameMove( float fTime, float g );//if reach a tank && m_State == 1, return the reached tank ID.else return 0.
	HRESULT Render();
	bool Launch();
	void SetModel( C3DTankModel *pBomb, C3DTankModel *pBombing );
	void SetTankControl( CTankControl *pTankControl ){ m_pTankControl = pTankControl; };
	BYTE GetState(){ return m_State; };
	void SetMovingSpeed( float fMvSpeed ){ m_fMovingSpeed = fMvSpeed; };
	void SetBombingSpeed( float fBmSpeed ){ m_fBombingSpeed = fBmSpeed; };
	void SetBombRange( float fBombRange ){ m_fBombRange = fBombRange; };
	float GetMovingSpeed(){ return m_fMovingSpeed; };
	float GetBombingSpeed(){ return m_fBombingSpeed; };
	float GetBombRange(){ return m_fBombRange; };
	D3DXVECTOR3* GetVector();
private:
	inline Vector2D* Identify( Vector2D *pVec )
	{
		float m = sqrt( pVec->x * pVec->x + pVec->y * pVec->y );
		pVec->x /= m;
		pVec->y /= m;
		return pVec;
	}
};
