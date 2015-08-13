#include "StdAfx.h"
#include "Util.h"
#include "Dmod.h"
#include "C3DTankModel.h"
#include "HeightMap.h"
#include "TankControl.h"

CTankControl::CTankControl( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pModel = NULL;
	m_pHeightMap = NULL;
	m_bMoving = false;
	m_bTeam = 1;
	m_dwID	= 1;
	m_lHP	= 100;
	D3DXMATRIX mtx( 1,0,0,0,
					0,1,0,0,
					0,0,1,0,
					1,1,1,1 );
	memcpy( &m_Matrix, &mtx, sizeof( D3DXMATRIX ) );
	m_pd3dDevice = pd3dDevice;
	m_MoveTo.x = 0;
	m_MoveTo.y = 0;
	m_LookAt = 0;
	m_bAIControl = true;
	m_fSpeed = 5.0f;
	Vector2D vTarget( -1.0f, -1.0f );
	SetAITarget( &vTarget );
	m_bSmartAI = false;
	m_dwAIRange = 5;
}

CTankControl::~CTankControl(void)
{
}
void CTankControl::SetMatrix( D3DXMATRIX *pMatrix )
{

	D3DXMATRIX mtx( 1,0,0,0,
					0,1,0,0,
					0,0,1,0,
					0,0,0,1 );
	if( pMatrix )
		memcpy( &m_Matrix, pMatrix, sizeof( D3DXMATRIX ) );
	else
		memcpy( &m_Matrix, &mtx, sizeof( D3DXMATRIX ) );
}
DWORD CTankControl::GetMark( DWORD x, DWORD y )
{
	if( !m_pHeightMap )
		return 0;
	if( x < m_pHeightMap->GetWidth() && y < m_pHeightMap->GetHeight() )
		return m_pMark[ x + y * m_pHeightMap->GetWidth() ]; 
	return 0;
}
bool CTankControl::SetPoint( DWORD x, DWORD y )
{
	if( !m_pHeightMap )
		return false;
	if( m_lHP <= 0 )
		return false;
	if( x >= m_pHeightMap->GetWidth() ||
		y >= m_pHeightMap->GetHeight() )
		return false;
	if( m_pMark[ x + y * m_pHeightMap->GetWidth() ] )
		return false;
	DWORD x1, y1;
	x1 = ( DWORD )( m_Matrix._41 + m_MoveTo.x );
	y1 = ( DWORD )( m_Matrix._43 + m_MoveTo.y );
	if( m_pMark[ x1 + y1 * m_pHeightMap->GetWidth() ] == m_dwID )
		m_pMark[ x1 + y1 * m_pHeightMap->GetWidth() ] = 0;
	m_bMoving = false;
	m_MoveTo.x = m_MoveTo.y = 0;
	m_Matrix._41 = (float)x;
	m_Matrix._43 = (float)y;
	UpDataHeight();
	m_pMark[ x + y * m_pHeightMap->GetWidth() ]  = m_dwID;
	return true;
}
bool CTankControl::SetLookAt(BYTE cLookAt)
{
	if( m_lHP <= 0 )
		return false;
	if( m_bMoving || m_MoveTo.x || m_MoveTo.y )
		return false;
	if( !m_pHeightMap )
		return false;
	BYTE look = cLookAt%4;
	m_LookAt = look;
	DWORD x,y,z;
	x = (DWORD)m_Matrix._41;
	y = (DWORD)m_Matrix._42;
	z = (DWORD)m_Matrix._43;
	D3DXMATRIX mtx( 1,0,0,0,
					0,1,0,0,
					0,0,1,0,
					0,0,0,1 );
	switch(look)
	{
	case 0:
		D3DXMatrixRotationY( &mtx, 0.0f );
		memcpy( &m_Matrix, &mtx, sizeof( float ) * 12 );
		if( z >= m_pHeightMap->GetHeight() - 2 )
			return false;
		if( m_pMark[ x + (z+1)*m_pHeightMap->GetWidth() ] ||
			m_pHeightMap->GetValue( x, z+1 ) - m_pHeightMap->GetValue( x, z ) > 10 )
			return false;
		m_MoveTo.x = 0;
		m_MoveTo.y = 1;
		break;
	case 1:
		D3DXMatrixRotationY( &mtx, D3DX_PI/2.0f );
		memcpy( &m_Matrix, &mtx, sizeof( float ) * 12 );
		if(  x >= m_pHeightMap->GetWidth() - 2 )
			return false;
		if( m_pMark[ x+1 + z*m_pHeightMap->GetWidth() ] ||
			m_pHeightMap->GetValue( x+1, z ) - m_pHeightMap->GetValue( x, z ) > 10 )
			return false;
		m_MoveTo.x = 1;
		m_MoveTo.y = 0;
		break;
	case 2:
		D3DXMatrixRotationY( &mtx, D3DX_PI );
		memcpy( &m_Matrix, &mtx, sizeof( float ) * 12 );
		if( z <= 1  )
			return false;
		if( m_pMark[ x + (z-1)*m_pHeightMap->GetWidth() ] ||
			m_pHeightMap->GetValue( x, z-1 ) - m_pHeightMap->GetValue( x, z ) > 10 )
			return false;
		m_MoveTo.x = 0;
		m_MoveTo.y = -1;
		break;
	case 3:
		D3DXMatrixRotationY( &mtx, 3.0f*D3DX_PI/2.0f );
		memcpy( &m_Matrix, &mtx, sizeof( float ) * 12 );
		if( x <= 1 )
			return false;
		if( m_pMark[ x-1 + z*m_pHeightMap->GetWidth() ] ||
			m_pHeightMap->GetValue( x-1, z ) - m_pHeightMap->GetValue( x, z ) > 10 )
			return false;
		m_MoveTo.x = -1;
		m_MoveTo.y = 0;
		break;
	}
	return true;
}
bool CTankControl::MoveToLookAt()
{
	if( !m_pHeightMap || !m_pMark )
		return false;
	if( m_MoveTo.x == 0&& m_MoveTo.y == 0 )
	{
		m_bMoving = false;
		return false;
	}
	if( m_bMoving || m_lHP <= 0)
		return false;
	m_bMoving = true;
	DWORD x = (DWORD)m_Matrix._41;
	DWORD z = (DWORD)m_Matrix._43;
	m_pMark[ x + z * m_pHeightMap->GetWidth() ] = 0;
	x += (long)(TOLONG(m_MoveTo.x) );
	z += (long)(TOLONG(m_MoveTo.y) );
	m_pMark[ x + z * m_pHeightMap->GetWidth() ] = m_dwID;
	return true;
}
void CTankControl::FrameMove( float fTime )
{
	if( m_lHP <= 0 )
	{
		DWORD x1, y1;
		DWORD Width = m_pHeightMap->GetWidth();
		x1 = (DWORD)(TOLONG(m_Matrix._41));
		y1 = (DWORD)(TOLONG(m_Matrix._43));
		DWORD *pdwMark = &(m_pMark[ x1 + y1 * Width ]);
		if(  *pdwMark == m_dwID )
			*pdwMark = 0;
		x1 += (long)(TOLONG(m_MoveTo.x) );
		y1 += (long)(TOLONG(m_MoveTo.y) );
		pdwMark = &(m_pMark[ x1 + y1 * Width ]);
		if(  *pdwMark == m_dwID )
			*pdwMark = 0;
		return;
	}
	if( fTime == 0.0f )
		return;
	float fS = fTime * m_fSpeed;
	if( fS > 0.49f )
		fS = 0.49f;
	AI();

	float c;
	float &x = m_MoveTo.x;
	float &z = m_MoveTo.y;
	if( x == 0 && z == 0 )
	{
		m_bMoving = false;
	}
	if( x || z )
	{
		c =  sqrt( x * x + z * z ) - fS ;
		if( c < 0 )
		{
			m_bMoving = false;
			x = z = 0;
			m_Matrix._41 = (float)(( DWORD )( m_Matrix._41 + 0.5f ));
			m_Matrix._43 = (float)(( DWORD )( m_Matrix._43 + 0.5f ));
		}
		if( m_bMoving )
		{
			if( x < 0 )
			{
				m_Matrix._41 -= fS;
				x += fS;
			}
			if( x > 0 )
			{
				m_Matrix._41 += fS;
				x -= fS;
			}
			if( z < 0 )
			{
				m_Matrix._43 -= fS;
				z += fS;
			}
			if( z > 0 )
			{
				m_Matrix._43 += fS;
				z -= fS;
			}
			
		}
	}
	UpDataHeight();
}
HRESULT CTankControl::Render()
{
	if( m_lHP <= 0 || !m_pModel )
		return S_OK;
	D3DXMATRIX mtx, mtx1;
	m_pd3dDevice->GetTransform( D3DTS_WORLD, &mtx );
	mtx1 = mtx * m_Matrix;
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mtx1 );
	HRESULT result = m_pModel->Render();
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mtx );
	return result;
}
void CTankControl::AI()
{
	static bool bFree = false;
	if( m_lHP <= 0 )
		return;
	
	if( !m_pHeightMap )
		return;
	if( m_bAIControl )
	{
		
		if( !m_bMoving )
		{
			DWORD i;

			if( !m_bSmartAI )
				goto try1;
			DWORD x = TOLONG(m_Matrix._41);
			DWORD y = TOLONG(m_Matrix._43);
			//Is player near enemy
			for( i = 1; i <= m_dwAIRange; i ++ )
			{
				if( x >= i && y >= i && x <= m_pHeightMap->GetWidth() - i
					&& y  <= m_pHeightMap->GetHeight() - i )
				{
					DWORD j;
					DWORD mark = 0;
					for( j = 0; j < 2 * i + 1; j++ )
					{
						mark |= m_pMark[ x - i + j + ( y + i ) * m_pHeightMap->GetWidth() ] == 1;
						mark |= m_pMark[ x - i + j + ( y - i ) * m_pHeightMap->GetWidth() ] == 1;
						mark |= m_pMark[ x + i + ( y - i + j ) * m_pHeightMap->GetWidth() ] == 1;
						mark |= m_pMark[ x - i + ( y - i + j ) * m_pHeightMap->GetWidth() ] == 1;
					}
					if( mark )
						AI_SearchPlayer();
					mark = m_pMark[ x + ( y + i ) * m_pHeightMap->GetWidth() ];
					if( mark && mark == 1 )
					{
						SetLookAt( 0 );
						goto mv;
					}
					mark = m_pMark[ x + ( y - i ) * m_pHeightMap->GetWidth() ];
					if( mark && mark == 1 )
					{
						SetLookAt( 2 );
						goto mv;
					}
					mark = m_pMark[  x + i  + y * m_pHeightMap->GetWidth() ];
					if( mark && mark == 1 )
					{
						SetLookAt( 1 );
						goto mv;
					}
					mark = m_pMark[  x - i  + y * m_pHeightMap->GetWidth() ];
					if( mark && mark == 1 )
					{
						SetLookAt( 3 );
						goto mv;
					}
				}
			}
			if(  m_AITarget.x >= 0 && m_AITarget.y >= 0 && !bFree )
			{
				if( !AI_SearchPlayer() )
					goto try1;
			}
			else
			{
try1:			bFree = true;
				int iRnd = rand();
				if( iRnd % 11 == 0 )
					goto rnd;
				if( iRnd % 7 == 0 )
					bFree = false;
				if( !SetLookAt( m_LookAt ) )
rnd:				for( i = 0 ; (i <= 10 && !SetLookAt( iRnd%4 )); i++  );
			}
		}
mv:		MoveToLookAt();
		
	}
}
bool CTankControl::AI_SearchPlayer()
{
	if( m_AITarget.x > m_Matrix._41 )
		if( !SetLookAt( 1 ) )
			goto t1;
	if( m_AITarget.x < m_Matrix._41 )
		if( !SetLookAt( 3 ) )
			goto t1;
t1:	if( m_AITarget.y > m_Matrix._43 )
		if( !SetLookAt( 0 ) )
			return false;
	if( m_AITarget.y < m_Matrix._43 )
		if( !SetLookAt( 2 ) )
			return false;
	return true;
}
void CTankControl::UpDataHeight()
{
	float v1 = m_pHeightMap->GetValue( (DWORD)(m_Matrix._41+0.5f), (DWORD)(m_Matrix._43+0.5f) );
	//float v2 = m_pHeightMap->GetValue( (DWORD)m_Matrix._41, (DWORD)m_Matrix._43 + 1 );
	//float v3 = m_pHeightMap->GetValue( (DWORD)m_Matrix._41 + 1, (DWORD)m_Matrix._43 );
	//float v4 = m_pHeightMap->GetValue( (DWORD)m_Matrix._41 + 1, (DWORD)m_Matrix._43 + 1 );
	//m_Matrix._42 = MAX( MAX ( v1 , v2 ),MAX( v3 , v4 ) ) * HEIGHTCOUNT;
	m_Matrix._42 = v1 * HEIGHTCOUNT;
}
