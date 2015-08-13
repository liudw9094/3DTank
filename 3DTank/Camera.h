#pragma once

class CCamera
{
protected:
	D3DXVECTOR3	m_vFrom;
	D3DXVECTOR3	m_vLookAt;
	D3DXVECTOR3	m_vUp;

	D3DXVECTOR3	m_vView;		//�����ָ��ĵ�λ��������
	D3DXVECTOR3	m_vCross;		//������Ĳ������� cross��view��up��

	D3DXMATRIX	m_matView;		//���������
	D3DXMATRIX	m_matBill;		//����ƾ���������������
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

	//����������ת
	D3DXMATRIX*	RotateLocalX( float angle );
	D3DXMATRIX*	RotateLocalY( float angle );
//	D3DXMATRIXA16*	RotateLocalZ( float angle );

	/// �ƶ�����������ϵ�� *pvֵ��λ��.
	D3DXMATRIX*	MoveTo( D3DXVECTOR3* pv );
	
	//��������ǰ��
	D3DXMATRIX*	MoveLocalX( float dist );
	D3DXMATRIX*	MoveLocalY( float dist );
	D3DXMATRIX*	MoveLocalZ( float dist );

};
