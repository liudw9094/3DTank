#pragma once


typedef struct _D3DTLVERTEX 
{
	D3DVECTOR Vector;
	D3DVECTOR Normal;
	float tu, tv;
}D3DTLVERTEX ;

typedef struct _Tex
{
	char FilePath[256];
	LPDIRECT3DTEXTURE9 m_pTexture;
} MyTexture;
class CTextureArray
{
private:
	DWORD m_Count;
	MyTexture *m_pTexture;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
public:
	CTextureArray( LPDIRECT3DDEVICE9 D3DDevice ){ m_Count = 0; m_pTexture = NULL; m_pd3dDevice = D3DDevice; };
	~CTextureArray();
	DWORD Create( LPSTR sFile );// 0 = error
	DWORD GetID( LPSTR sFile );//0 = error or none
	LPDIRECT3DTEXTURE9 GetTexture( DWORD ID );
	LPDIRECT3DTEXTURE9 GetTexture( LPSTR sFile );
private:
	void Release();
};/*
class CTextureID
{
public:
	DWORD	m_DiffuseMap;
	DWORD	m_SpecularMap;
	DWORD	m_BumpMap;
	DWORD	m_DisplacementMap;
public:
	CTextureID(){ m_DiffuseMap = m_SpecularMap = m_BumpMap = m_DisplacementMap = 0L; };
	~CTextureID(){};

};*/
class C3DTankModel :
	public MyModel
{
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 *m_ppVB;
	LPDIRECT3DINDEXBUFFER9	*m_ppIB;
	//LPD3DXMESH m_pD3DMesh;
	char m_FileName[256];
	char m_ModelDir[256];
	char m_TextureDir[256];
	char m_FilePath[256];
public:
	DWORD			D3DFVF;
	//CTextureID		*m_FrameTextureID;
	CTextureArray	*m_TextureList;
	C3DTankModel( LPDIRECT3DDEVICE9 D3DDevice );
	~C3DTankModel(void);
	HRESULT Render();
	bool Create( char *FilePath );
	void Release();
	HRESULT InitVBnIB();
	HRESULT ReleaseVBnIB();
	DWORD GetTextureFromFile( LPSTR sFilePath, BYTE bKey );
	void SetDir( LPSTR ModelDir, LPSTR TextureDir );//设置模型和材质目录
	void Reset();
private:
	HRESULT FrameToD3DTLVERTEX( D3DTLVERTEX *(&pOut), DWORD dwIndex );
	inline void SetMaterial( DWORD FrameIndex )
	{
		D3DMATERIAL9 DefaultMaterial = {	{ 0.5f, 0.5f, 0.5f, 1.0f },
										{ 0.5f, 0.5f, 0.5f, 1.0f },
										{ 1.0f, 1.0f, 1.0f, 1.0f },
										{ 0.0f, 0.0f, 0.0f, 0.0f },
										0.0f };
		if( m_Frames[FrameIndex].m_pMaterialID )
		{
			if( *m_Frames[FrameIndex].m_pMaterialID )
			{
				DWORD MaterialIndex = *m_Frames[FrameIndex].m_pMaterialID - 1;
				m_pd3dDevice->SetTexture( 0, m_TextureList->GetTexture( m_pMaterial[MaterialIndex].m_DiffuseMap ) );//设置贴图
				//m_pd3dDevice->SetTexture( 1, m_TextureList->GetTexture( m_pMaterial[MaterialIndex].m_SpecularMap ) );
				//m_pd3dDevice->SetTexture( 2, m_TextureList->GetTexture( m_pMaterial[MaterialIndex].m_BumpMap ) );
				//m_pd3dDevice->SetTexture( 3, m_TextureList->GetTexture( m_pMaterial[MaterialIndex].m_DisplacementMap ) );
				m_pd3dDevice->SetMaterial( (D3DMATERIAL9 *)&(m_pMaterial[*m_Frames[FrameIndex].m_pMaterialID]) );//设置材质


				/*m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
				m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_SPECULAR );
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );*/
			}
		}
		else
		{
			m_pd3dDevice->SetMaterial( &DefaultMaterial );//设置材质
			m_pd3dDevice->SetTexture( 0, NULL  );
		}
	};
};

