#pragma once

#define MAP_NAME "Map"
#define MAP_HEIGHT "HeightMap"
#define MAP_TEXTURE "Texture"
#define MAP_SKY "Sky"
#define HEIGHTCOUNT (1.0f/8.0f)
#define MAP2D_HEIGHT 100.0f
#define MAP2D_WIDTH 100.0f
struct HEIGHT_VERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR3		n;
	D3DXVECTOR2		t;
};
#define D3DFVF_HEIGHT_VERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
struct VERTEX2D
{
	D3DXVECTOR4		p;
	D3DCOLOR		c;
	D3DXVECTOR2		t;
};
#define D3DFVF_VERTEX2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)


struct SKY_VERTEX
{
	float x , y , z ;
	float tu, tv;
};
#define D3DFVF_SKY_VERTEX  (D3DFVF_XYZ |D3DFVF_TEX1 )

struct HEIGHT_INDEX
{
	DWORD _0, _1, _2;
};
class CHeightMap
{
private:
	BYTE *m_pValue;
	DWORD m_Height, m_Width;
	DWORD m_VertCount, m_FaceCount;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
public:
	CHeightMap( LPDIRECT3DDEVICE9 pd3dDevice );
	~CHeightMap(void);
	bool Create( LPSTR strFilePath );
	void Release();
	BYTE GetValue( DWORD index );
	BYTE GetValue( DWORD u, DWORD v );
	DWORD GetHeight(){ return m_Height; };
	DWORD GetWidth(){ return m_Width; };
	HRESULT Render();
	inline BYTE operator [] ( DWORD index ){ return GetValue( index ); };
	inline BYTE operator () ( DWORD index ){ return GetValue( index ); };
	inline BYTE operator () ( DWORD u, DWORD v ){ return GetValue( u, v ); };
	bool InitVBnIB();
	void ReleaseVBnIB();

};

class CMap
{
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	CHeightMap *m_pHeightMap;
	D3DMATERIAL9 m_Material;
	LPDIRECT3DTEXTURE9 m_pTexture;
	//LPDIRECT3DVERTEXBUFFER9 m_pSkyVB;
	//LPDIRECT3DINDEXBUFFER9	m_pSkyIB;
	LPDIRECT3DVERTEXBUFFER9 m_p2DVB;
	C3DTankModel *m_pSky;
	char m_MapDir[256],m_HeightMapDir[256],m_TextureDir[256],m_MapFile[256];
public:
	CMap( LPDIRECT3DDEVICE9 pd3dDevice );
	~CMap();
	void SetDir( LPSTR sMapDir = NULL,
				LPSTR sHeightMapDir = NULL,
				LPSTR sTextureDir = NULL );
	void SetMaterial( LPD3DXMATERIAL Material );
	bool Create( LPSTR sINIFilePath );
	void Release();
	CHeightMap* GetHeightMap(){ return m_pHeightMap; };
	D3DMATERIAL9* GetMaterial(){ return &m_Material; };
	LPDIRECT3DTEXTURE9 GetTexture(){ return m_pTexture; };
	HRESULT Render();
	HRESULT Render2D();
	void Reset();
	bool InitVBnIB();
	void ReleaseVBnIB();
};