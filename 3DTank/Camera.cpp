#include "StdAfx.h"
#include "Camera.h"

CCamera::CCamera(void)
{
	D3DXVECTOR3	from(0.0f,0.0f,0.0f);
	D3DXVECTOR3	lookat(0.0f,0.0f,-1.0f);
	D3DXVECTOR3	up(0.0f,1.0f,0.0f);
	D3DXMatrixIdentity( &m_matView );
	D3DXMatrixIdentity( &m_matBill );
	SetView( &from, &lookat, &up );
}

CCamera::~CCamera(void)
{
}

//设定用于创建摄像机矩阵的基本向量值.
D3DXMATRIX*	CCamera::SetView( D3DXVECTOR3* pvFrom,D3DXVECTOR3* pvLookAt,D3DXVECTOR3* pvUp)
{
	m_vFrom		= *pvFrom;
	m_vLookAt	= *pvLookAt;
	m_vUp		= *pvUp;
	D3DXVec3Normalize( &m_vView, &( m_vLookAt - m_vFrom ) );
	D3DXVec3Cross( &m_vCross, &m_vUp, &m_vView );

	D3DXMatrixLookAtLH( &m_matView, &m_vFrom, &m_vLookAt, &m_vUp);
	D3DXMatrixInverse( &m_matBill, NULL, &m_matView );
	m_matBill._41 = 0.0f;
	m_matBill._42 = 0.0f;
	m_matBill._43 = 0.0f;

	return &m_matView;
}

//以摄像机坐标系的 X轴为准旋转一定的角度 angle.
D3DXMATRIX* CCamera::RotateLocalX( float angle )
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis( &matRot, &m_vCross, angle );

	D3DXVECTOR3 vNewDst,vNewUp;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );	// 通过view * rot求得新的 dst vector.
//	D3DXVec3Cross( &vNewUp, &vNewDst, &m_vCross );			// 通过cross( dst, x轴)求得 up vector.
//	D3DXVec3Normalize( &vNewUp, &vNewUp );					// up vector变为 unit vector...
	vNewDst += m_vFrom;										// 实际 dst position =  From Position + dst vector

	return SetView( &m_vFrom, &vNewDst, &m_vUp );
}

//以摄像机坐标系的 Y轴为准旋转一定的角度 angle.
D3DXMATRIX* CCamera::RotateLocalY( float angle )
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis( &matRot, &m_vUp, angle );

	D3DXVECTOR3 vNewDst;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );	// 通过view * rot求得新的 dst vector.
	vNewDst += m_vFrom;										// 实际 dst position =  From Position + dst vector

	return SetView( &m_vFrom, &vNewDst, &m_vUp );
}

//按照摄像机坐标系 X轴方向前进 dist.(后退时，可以输入 -dist.)
D3DXMATRIX* CCamera::MoveLocalX( float dist )
{
	D3DXVECTOR3 vNewFrom	= m_vFrom;
	D3DXVECTOR3 vNewDst	= m_vLookAt;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize( &vMove, &m_vCross );
	vMove	*= dist;
	vNewFrom += vMove;
	vNewDst += vMove;

	return SetView( &vNewFrom, &vNewDst, &m_vUp );
}

/// 按照摄像机坐标系 Y轴方向前进 dist.(后退时，可以输入 -dist.)
D3DXMATRIX* CCamera::MoveLocalY( float dist )
{
	D3DXVECTOR3 vNewFrom	= m_vFrom;
	D3DXVECTOR3 vNewDst	= m_vLookAt;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize( &vMove, &m_vUp );
	vMove	*= dist;
	vNewFrom += vMove;
	vNewDst += vMove;

	return SetView( &vNewFrom, &vNewDst, &m_vUp );
}

//按照摄像机坐标系 Z轴方向前进 dist.(后退时，可以输入 -dist.)
D3DXMATRIX* CCamera::MoveLocalZ( float dist )
{
	D3DXVECTOR3 vNewFrom	= m_vFrom;
	D3DXVECTOR3 vNewDst	= m_vLookAt;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize( &vMove, &m_vView );
	vMove	*= dist;
	vNewFrom += vMove;
	vNewDst += vMove;

	return SetView( &vNewFrom, &vNewDst, &m_vUp );
}

//把摄像机移动至世界坐标系的 *pv值的位置.
D3DXMATRIX* CCamera::MoveTo( D3DXVECTOR3* pv )
{
	D3DXVECTOR3	dv = *pv - m_vFrom;
	m_vFrom = *pv;
	m_vLookAt += dv;
	return SetView( &m_vFrom, &m_vLookAt, &m_vUp );
}
