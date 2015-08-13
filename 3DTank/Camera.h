#pragma once

class CCamera
{
protected:
	D3DXVECTOR3	m_vFrom;
	D3DXVECTOR3	m_vLookAt;
	D3DXVECTOR3	m_vUp;

	D3DXVECTOR3	m_vView;		//摄像机指向的单位方向向量
	D3DXVECTOR3	m_vCross;		//摄像机的侧面向量 cross（view，up）

	D3DXMATRIX	m_matView;		//摄像机矩阵
	D3DXMATRIX	m_matBill;		//广告牌矩阵（摄像机的逆矩阵）
public:
	CCamera(void);
	~CCamera(void);

	D3DXMATRIX*	SetView( D3DXVECTOR3* pvFrom, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp);
	D3DXMATRIX*	GetViewMatrix(){ return &m_matView; };
	D3DXMATRIX*	GetBillboardMatrix(){ return &m_matBill; };

	void SetFromV( D3DXVECTOR3 *pvFrom ){ m_vFrom = *pvFrom; };
	void SetLookAtV( D3DXVECTOR3 *pvLookAt ){ m_vLookAt = *pvLookAt; };
	void SetUpV( D3DXVECTOR3 *pvUp ){ m_vUp = *pvUp; };
	D3DVECTOR *GetFromV(){ return &m_vFrom; };
	D3DVECTOR *GetLookAtV(){ return &m_vLookAt; };
	D3DVECTOR *GetUpV(){ return &m_vUp; };

	//
	void UpData(){ SetView( &m_vFrom, &m_vLookAt, &m_vUp ); };

	//绕坐标轴旋转
	D3DXMATRIX*	RotateLocalX( float angle );
	D3DXMATRIX*	RotateLocalY( float angle );
//	D3DXMATRIXA16*	RotateLocalZ( float angle );

	/// 移动到世界坐标系的 *pv值的位置.
	D3DXMATRIX*	MoveTo( D3DXVECTOR3* pv );
	
	//沿坐标轴前进
	D3DXMATRIX*	MoveLocalX( float dist );
	D3DXMATRIX*	MoveLocalY( float dist );
	D3DXMATRIX*	MoveLocalZ( float dist );

};
