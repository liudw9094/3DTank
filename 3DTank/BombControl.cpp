#include "StdAfx.h"
#include "Util.h"
#include "Dmod.h"
#include "C3DTankModel.h"
#include "HeightMap.h"
#include "TankControl.h"
#include "BombControl.h"

CBombControl::CBombControl( LPDIRECT3DDEVICE9 pd3dDevice )
{
	D3DXMATRIX mat( 1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f );
	m_BombMatrix = mat;
	m_pd3dDevice = pd3dDevice;
	m_pBomb = m_pBombing = NULL;
	m_State = 0;
	m_fMovingSpeed = 17.0f;
	m_fBombingSpeed = 4.0f;
	m_fBombRange = 4.0f;
	m_fVG = 0.0f;
}

CBombControl::~CBombControl(void)
{
}
bool CBombControl::Launch()
{
	if( m_State != 0 )
		return false;
	float x = m_pTankControl->GetMatrix()->_41;
	float y = m_pTankControl->GetMatrix()->_42 + 0.5f;
	float z = m_pTankControl->GetMatrix()->_43;
	D3DXMATRIX mat( 1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x, y, z, 1.0f );
	m_BombMatrix = mat;
	BYTE lookat = m_pTankControl->GetLookAt() % 4;
	switch( lookat )
	{
	case 0:
		m_LookAt.x = 0;
		m_LookAt.y = 1;
		break;
	case 1:
		m_LookAt.x = 1;
		m_LookAt.y = 0;
		break;
	case 2:
		m_LookAt.x = 0;
		m_LookAt.y = -1;
		break;
	case 3:
		m_LookAt.x = -1;
		m_LookAt.y = 0;
		break;
	}
	Identify( &m_LookAt );
	m_State = 1;
	m_fVG = 0.0f;
	return true;
}
DWORD CBombControl::FrameMove( float fTime, float g )
{
	if( m_State == 0 )
		return 0;
	float move = fTime;
	D3DXMATRIX &mat = m_BombMatrix;
	if( m_State == 1 && m_pTankControl->GetMap() )
	{
		m_fVG += g * move;
		mat._42 -= m_fVG;
		mat._41 += m_LookAt.x * move * m_fMovingSpeed;
		mat._43 += m_LookAt.y * move * m_fMovingSpeed;
		DWORD x = (DWORD)( mat._41 + 0.5f );
		DWORD y = (DWORD)( mat._43 + 0.5f );
		if( mat._41 >= 0 && mat._41 <= m_pTankControl->GetMap()->GetWidth() &&
			mat._43 >= 0 && mat._43 <= m_pTankControl->GetMap()->GetHeight() )
		{
			if( mat._42 <= m_pTankControl->GetMap()->GetValue( x, y ) * HEIGHTCOUNT )
				m_State = 2;
			DWORD mark = m_pTankControl->GetMark( x, y );
			if( mark && mark != m_pTankControl->GetID() &&
				mat._42 >= m_pTankControl->GetMap()->GetValue( x, y ) * HEIGHTCOUNT &&
				mat._42 <= m_pTankControl->GetMap()->GetValue( x, y ) * HEIGHTCOUNT + 2.0f )
			{
				m_State = 2;
				return mark;
			}
		}
		else
			m_State = 2;
	}
	if( m_State == 2 )
	{
		
		mat._11 = mat._22 = mat._33 = mat._11 + move * m_fBombingSpeed;
		if( mat._11 >= m_fBombRange )
			m_State = 0;
	}
	return 0;
}
HRESULT CBombControl::Render()
{
	if( m_State == 0 )
		return S_OK;
	D3DXMATRIX mat;
	m_pd3dDevice->GetTransform( D3DTS_WORLD, &mat );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_BombMatrix );
	if( m_State == 1 && m_pBomb )
		m_pBomb->Render();
	if( m_State == 2 && m_pBombing )
		m_pBombing->Render();
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
	return S_OK;
}
void CBombControl::SetModel( C3DTankModel *pBomb, C3DTankModel *pBombing )
{
	m_pBomb = pBomb;
	m_pBombing = pBombing;
}
D3DXVECTOR3* CBombControl::GetVector()
{
	static D3DXVECTOR3 vec;
	if( m_State == 0 )
	{
		vec.x = m_pTankControl->GetMatrix()->_41;
		vec.y = m_pTankControl->GetMatrix()->_42 + 0.5f;
		vec.z = m_pTankControl->GetMatrix()->_43;
	}
	else
	{
		vec.x = m_BombMatrix._41;
		vec.y = m_BombMatrix._42;
		vec.z = m_BombMatrix._43;
	}
	return &vec;
}