#include "StdAfx.h"
#include "Util.h"
#include "dmod.h"
#include "C3DTankModel.h"
#include "HeightMap.h"

CHeightMap::CHeightMap( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	m_pValue = NULL;
	m_pVB = NULL;
	m_pIB = NULL;
	m_Height = m_Width = 0;
}
CHeightMap::~CHeightMap(void)
{
	Release();
}
bool CHeightMap::Create( LPSTR strFilePath )
{
	if( m_pValue || m_Height || m_Width || m_pVB ||m_pIB )
		return false;
	LPDIRECT3DTEXTURE9 pHeight;
	if( FAILED( D3DXCreateTextureFromFile ( m_pd3dDevice, strFilePath, &pHeight ) ) )
		return false;
	D3DSURFACE_DESC		ddsd;
	D3DLOCKED_RECT		d3drc;

	pHeight->GetLevelDesc( 0, &ddsd );	/// 纹理信息
	m_Width = ddsd.Width;				/// 纹理的长度
	m_Height = ddsd.Height;				/// 纹理的宽度
	m_pValue = new BYTE[ m_Height * m_Width ];
	BYTE *temp = m_pValue;

	/// 纹理存储器!
	if( FAILED( pHeight->LockRect( 0, &d3drc, NULL, D3DLOCK_READONLY ) ) )
		return false;

	for( DWORD z = 0 ; z < m_Height ; z++ )
	{
		for( DWORD x = 0 ; x < m_Width ; x++ )
		{
			*(temp++) = ((BYTE)(*((LPDWORD)d3drc.pBits+x+z*(d3drc.Pitch/4))&0x000000ff));	/// 由于是DWORD，因此为pitch/4
			
		}
	}

	if( FAILED( pHeight->UnlockRect( 0 ) ) )
		return false;
	SAFE_RELEASE( pHeight );
	return InitVBnIB();
}
void CHeightMap::Release()
{
	SAFE_DELETE_ARRAY( m_pValue );
	ReleaseVBnIB();
	m_Height = m_Width = 0;
}
BYTE CHeightMap::GetValue( DWORD index )
{
	if( m_pValue )
		return m_pValue[index];
	return 0;
}
BYTE CHeightMap::GetValue( DWORD u, DWORD v )
{
	if( m_pValue )
		return m_pValue[ u + (GetHeight() - 1 - v) * m_Width ];
	return 0;
}
bool CHeightMap::InitVBnIB()
{
	if( (!m_pValue) || (!m_Height) || (!m_Width) || m_pVB ||m_pIB )
		return false;
	//===============InitVB===========
	m_VertCount = m_Width*m_Height;
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_VertCount*sizeof(HEIGHT_VERTEX),
												  0, D3DFVF_HEIGHT_VERTEX,
												  D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return false;
    }
    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
        return false;

	HEIGHT_VERTEX	v;
	HEIGHT_VERTEX*	pV = (HEIGHT_VERTEX*)pVertices;
	for( DWORD z = 0 ; z < m_Height ; z++ )
	{
		for( DWORD x = 0 ; x < m_Width ; x++ )
		{
			v.p.x = (float)x;		// 顶点的x坐标（以原点为准对齐网格）
			v.p.z = (float)(m_Height-z) - 1;					// 顶点的z坐标（以顶点为准对齐网格）.
			v.p.y = GetValue( x + z * m_Width ) * HEIGHTCOUNT;
			v.n.x = v.p.x;
			v.n.y = v.p.y;
			v.n.z = v.p.z;
			D3DXVec3Normalize( &v.n, &v.n );
			v.t.x = (float)x / (m_Width-1);
			v.t.y = (float)z / (m_Height-1);
			*pV++ = v;
		}
	}
    m_pVB->Unlock();
	//==============InitIB===========
	m_FaceCount = (m_Width-1)*(m_Height-1)*2;
    if( FAILED( m_pd3dDevice->CreateIndexBuffer( 
				m_FaceCount * sizeof(HEIGHT_INDEX),
				0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
    {
        return false;
    }

	HEIGHT_INDEX i;
    HEIGHT_INDEX* pI;
    if( FAILED( m_pIB->Lock( 0, 0, (void**)&pI, 0 ) ) )
        return false;

	for( DWORD z = 0 ; z < m_Height-1 ; z++ )
	{
		for( DWORD x = 0 ; x < m_Width-1 ; x++ )
		{
			i._0 = (z*m_Width+x);
			i._1 = (z*m_Width+x+1);
			i._2 = ((z+1)*m_Width+x);
			*pI++ = i;
			i._0 = ((z+1)*m_Width+x);
			i._1 = (z*m_Width+x+1);
			i._2 = ((z+1)*m_Width+x+1);
			*pI++ = i;
		}
	}
    m_pIB->Unlock();
	return true;
}
void CHeightMap::ReleaseVBnIB()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );
}
HRESULT CHeightMap::Render()
{

	if( !m_pVB || !m_pIB || !m_VertCount || !m_FaceCount )
		return S_FALSE;



	m_pd3dDevice->SetFVF( D3DFVF_HEIGHT_VERTEX );//设置FVF
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0 , sizeof( HEIGHT_VERTEX) );//载入顶点缓冲
	m_pd3dDevice->SetIndices( m_pIB );//载入面缓冲
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0, 0, 
										m_VertCount, 0, m_FaceCount );
	return S_OK;
}

CMap::CMap(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pHeightMap = new CHeightMap( pd3dDevice );
	m_pd3dDevice = pd3dDevice;
	m_MapDir[0] = '\0';
	m_HeightMapDir[0] = '\0';
	m_TextureDir[0] = '\0';
	m_MapFile[0] = '\0';
	m_pTexture = NULL;
	//m_pSkyVB = NULL;
	//m_pSkyIB = NULL;
	m_p2DVB = NULL;
	m_pSky = NULL;
}
CMap::~CMap()
{
	Release();
	delete m_pHeightMap;
}
void CMap::SetDir( LPSTR sMapDir, LPSTR sHeightMapDir, LPSTR sTextureDir )
{
	if( sMapDir )
	{
		if( strrchr( sMapDir, '\\' )[0] != '\0' )
			sprintf( m_MapDir,"%s\\", sMapDir );
		else
			strcpy( m_MapDir, sMapDir );
	}
	else  m_MapDir[0] = '\0' ;
	if( sHeightMapDir )
	{
		if( strrchr( sHeightMapDir, '\\' )[0] != '\0' )
			sprintf( m_HeightMapDir,"%s\\", sHeightMapDir );
		else
			strcpy( m_HeightMapDir, sHeightMapDir );
	}
	else  m_HeightMapDir[0] = '\0' ;
	if( sTextureDir )
	{
		if( strrchr( sTextureDir, '\\' )[0] != '\0' )
			sprintf( m_TextureDir,"%s\\", sTextureDir );
		else
			strcpy( m_TextureDir, sTextureDir );
	}
	else  m_TextureDir[0] = '\0' ;
	//sHeightMapDir ? strcpy( m_HeightMapDir, sHeightMapDir ) : &( m_HeightMapDir[0] = '\0' );
	//sTextureDir  ? strcpy( m_TextureDir, sTextureDir  ) : &( m_TextureDir[0] = '\0' );


}
bool CMap::Create(LPSTR sINIFilePath)
{
	if( sINIFilePath != m_MapFile )
		strcpy( m_MapFile, sINIFilePath );
	if( m_pTexture )
		return false;
	char sHMap[256], sTexture[256], sSky[256];
	char sHMap_1[256], sTexture_1[256], sSky_1[256];
	char sMapPath[256];
	if( sINIFilePath[1] != ':' )
		sprintf( sMapPath, "%s%s", m_MapDir, sINIFilePath );
	GetPrivateProfileString( MAP_NAME, MAP_HEIGHT, "MapDefault.bmp", sHMap_1, 256, sMapPath );
	GetPrivateProfileString( MAP_NAME, MAP_TEXTURE, "TextureDefault.tga", sTexture_1, 256, sMapPath );
	GetPrivateProfileString( MAP_NAME, MAP_SKY, "SkyDefault.dmod", sSky_1, 256, sMapPath );
	if( sHMap_1[1] != ':' )
		sprintf( sHMap, "%s%s", m_HeightMapDir, sHMap_1 );
	else
		sprintf( sHMap, "%s", sHMap_1 );
	if( sTexture_1[1] != ':' )
		sprintf( sTexture, "%s%s", m_TextureDir, sTexture_1 );
	else
		sprintf( sTexture, "%s", sTexture_1 );
	if( sSky_1[1] != ':' )
		sprintf( sSky, "%s%s", m_MapDir, sSky_1 );
	else
		sprintf( sSky, "%s", sSky_1 );
	// 创建材质（material）
	// 在设备中创建一个材质.
	ZeroMemory( &m_Material, sizeof(D3DMATERIAL9) );
	m_Material.Diffuse.r = m_Material.Ambient.r =
	m_Material.Diffuse.g = m_Material.Ambient.g =
	m_Material.Diffuse.b = m_Material.Ambient.b =
	m_Material.Diffuse.a = m_Material.Ambient.a = 1.0f;
	
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, sTexture, &m_pTexture ) ) )
		return false;
	if( !( m_pHeightMap->Create( sHMap ) ) )
		return false;

	m_pSky = new C3DTankModel( m_pd3dDevice );
	m_pSky->SetDir( m_MapDir, m_TextureDir );
	if( !m_pSky->Create( sSky ) )
		return false;

	return InitVBnIB();

}
void CMap::Release()
{
	ReleaseVBnIB();
	m_pHeightMap->Release();
	SAFE_RELEASE( m_pTexture );

	SAFE_DELETE( m_pSky );
}
HRESULT CMap::Render()
{
	m_pd3dDevice->SetMaterial( &m_Material );
	
	//Render Sky
	DWORD state,state1,state2;
	D3DMATRIX mtx, mtx1;
	m_pd3dDevice->GetTransform( D3DTS_VIEW, &mtx );
	memcpy( &mtx1, &mtx, sizeof(D3DMATRIX) );
	mtx._41 = mtx._42 = mtx._43 = 0.0f;
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &mtx );
	m_pd3dDevice->GetRenderState( D3DRS_ZENABLE, &state );
	m_pd3dDevice->GetRenderState( D3DRS_CULLMODE, &state1 );
	m_pd3dDevice->GetRenderState( D3DRS_LIGHTING, &state2 );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	//m_pd3dDevice->SetTexture( 0, m_pSkyTexture );
	/**
	m_pd3dDevice->SetFVF( D3DFVF_SKY_VERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pSkyVB, 0, sizeof( SKY_VERTEX ) );
	m_pd3dDevice->SetIndices( m_pSkyIB );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12 );*/
	m_pSky->Render();
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, state );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, state1 );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, state2);
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &mtx1 );

	m_pd3dDevice->SetTexture( 0, m_pTexture );
	m_pd3dDevice->SetMaterial( &m_Material );
	return m_pHeightMap->Render();
}
HRESULT CMap::Render2D()
{
	m_pd3dDevice->SetTexture( 0, m_pTexture );
	
	m_pd3dDevice->SetFVF( D3DFVF_VERTEX2D );
	m_pd3dDevice->SetStreamSource( 0, m_p2DVB, 0, sizeof( VERTEX2D ) );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN , 0, 2 );
	return S_OK;
}
void CMap::Reset()
{
	if( m_MapFile[0] != '\0' )
	{
		Release();
		Create( m_MapFile );
	}
}
bool CMap::InitVBnIB()
{
	if( m_pHeightMap )
		m_pHeightMap->InitVBnIB();
	if( m_pSky )
		m_pSky->InitVBnIB();
		/*
	//=========== Init Sky VB =============
	SKY_VERTEX v[24];
	
	v[0].y = v[1].y = v[2].y = v[3].y =
	v[4].y = v[5].y = v[10].y = v[11].y =
	v[17].y = v[18].y = v[20].y = v[23].y = -2;
	v[6].y = v[7].y = v[8].y = v[9].y =
	v[12].y = v[13].y = v[14].y = v[15].y =
	v[16].y = v[19].y = v[21].y = v[22].y = 2;
	
	v[2].x = v[3].x = v[4].x = v[5].x =
	v[6].x = v[7].x = v[12].x = v[13].x =
	v[18].x = v[19].x = v[22].x = v[23].x = -2;
	v[0].x = v[1].x = v[8].x = v[9].x =
	v[10].x = v[11].x = v[14].x = v[15].x =
	v[16].x = v[17].x = v[20].x = v[21].x = 2;
	
	v[1].z = v[2].z = v[5].z = v[6].z =
	v[9].z = v[10].z = v[13].z = v[14].z =
	v[20].z = v[21].z = v[22].z = v[23].z = -2;
	v[0].z = v[3].z = v[4].z = v[7].z =
	v[8].z = v[11].z = v[12].z = v[15].z =
	v[16].z = v[17].z = v[18].z = v[19].z = 2;
	
	v[6].tu = v[7].tu =0;
	v[2].tu= v[3].tu =v[4].tu =v[5].tu =v[18].tu= v[19].tu= v[22].tu = v[23].tu = 0.25f;
	v[0].tu = v[1].tu =v[10].tu = v[11].tu = v[16].tu=v[17].tu=v[20].tu=v[21].tu = 0.5f;
	v[8].tu = v[9].tu = v[14].tu = v[15].tu = 0.75f;
	v[12].tu = v[13].tu = 1.0f;
	v[16].tv = v[19].tv = 0;
	v[0].tv = v[3].tv = v[4].tv = v[7].tv = v[8].tv = v[11].tv = v[12].tv = v[15].tv = v[17].tv = v[18].tv = (1.0f/3.0f);
	v[1].tv = v[2].tv = v[5].tv = v[6].tv = v[9].tv = v[10].tv = v[13].tv = v[14].tv = v[20].tv = v[23].tv = (2.0f/3.0f);
	v[21].tv = v[22].tv = 1.0f;
	
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( sizeof(v), 0,
				D3DFVF_SKY_VERTEX,D3DPOOL_DEFAULT, &m_pSkyVB, NULL ) ) )
		return false;
	void *temp;
	m_pSkyVB->Lock( 0, 0, &temp, 0 );
	memcpy( temp, v, sizeof(v) );
	m_pSkyVB->Unlock();
	//=========== Init Sky IB =============
	HEIGHT_INDEX face[12];
	DWORD i;
	for( i = 0; i < 6; i++ )
	{
		DWORD t = 2 * i;
		face[t]._0 = i*4; face[t]._1 = i*4+1; face[t]._2 = i*4+2;
		t++;
		face[t]._0 = i*4; face[t]._1 = i*4+2; face[t]._2 = i*4+3;
	}
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( sizeof( face ), 0,
				D3DFMT_INDEX32 ,D3DPOOL_DEFAULT , &m_pSkyIB, NULL ) ) )
		return false;
	m_pSkyIB->Lock( 0, 0, &temp, 0 );
	memcpy( temp, face, sizeof( face ) );
	m_pSkyIB->Unlock();*/

	if( m_p2DVB )
		return true;
	VERTEX2D v[4];
	v[0].p.w = v[1].p.w = v[2].p.w = v[3].p.w = 1.0f;
	v[0].p.z = v[1].p.z = v[2].p.z = v[3].p.z = 0.5f;
	v[0].p.x = v[1].p.x = MAP2D_WIDTH;
	v[2].p.x = v[3].p.x = 0.0f;
	v[1].p.y = v[2].p.y = MAP2D_HEIGHT;
	v[0].p.y = v[3].p.y = 0.0f;
	v[0].t.x = v[1].t.x = 1.0f;
	v[2].t.x = v[3].t.x = 0.0f;
	v[1].t.y = v[2].t.y = 1.0f;
	v[0].t.y = v[3].t.y = 0.0f;
	v[0].c = v[1].c = v[2].c = v[3].c = D3DCOLOR_ARGB( 177, 177 ,177 , 177);
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( sizeof(v), 0,
				D3DFVF_VERTEX2D,D3DPOOL_DEFAULT, &m_p2DVB, NULL ) ) )
		return false;
	void *temp;
	m_p2DVB->Lock( 0, 0, &temp, 0 );
	memcpy( temp, v, sizeof(v) );
	m_p2DVB->Unlock();
	return true;
}
void CMap::ReleaseVBnIB()
{
	if( m_pSky )
		m_pSky->ReleaseVBnIB();
	SAFE_RELEASE( m_p2DVB );
	m_pHeightMap->ReleaseVBnIB();
}