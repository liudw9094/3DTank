#include "StdAfx.h"
#include "Util.h"
#include "DMod.h"
#include "C3DTankModel.h"



CTextureArray::~CTextureArray()
{
	if( m_pTexture )
	{
		DWORD i;
		for( i = 0 ; i< m_Count; i++ )
		{
			m_pTexture[i].m_pTexture->Release();
		}
		SAFE_DELETE_ARRAY( m_pTexture );
	}
}
DWORD CTextureArray::GetID( LPSTR sFile )
{
	DWORD i;
	if( m_Count )
		for( i = 0; i< m_Count; i++ )
			if( !strcmp( m_pTexture[i].FilePath, sFile ) )
				return i+1;

	return 0;
}
DWORD CTextureArray::Create( LPSTR sFile )
{
	DWORD ID = GetID(sFile);
	if( ID )
		return ID;

	ID = m_Count + 1;
	MyTexture *temp = new MyTexture[ ID ];
	memcpy( temp, m_pTexture, ( m_Count )*sizeof( MyTexture ) );
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, sFile,
		&( temp[ m_Count ].m_pTexture) ) ) )//为优化算法，用m_Count代替ID-1
	{
		#ifdef _DEBUG 
			char err[256];
			sprintf( err ,"Error on loading texture: %s",sFile );
			MessageBoxEx( err );
		#endif
		delete [] temp;
		return 0;
	}
	strcpy( temp[ m_Count ].FilePath , sFile );
	MyTexture *ReleaseTexture = m_pTexture;
	m_pTexture = temp;
	delete [] ReleaseTexture;
	m_Count = ID;
	return ID;
}
LPDIRECT3DTEXTURE9 CTextureArray::GetTexture(DWORD ID)
{
	if( ID > m_Count || ID == 0 )
		return NULL;
	return m_pTexture[ ID - 1 ].m_pTexture;
}
LPDIRECT3DTEXTURE9 CTextureArray::GetTexture( LPSTR sFile )
{
	DWORD ID = GetID( sFile );
	return GetTexture( ID );
}
C3DTankModel::C3DTankModel( LPDIRECT3DDEVICE9 D3DDevice )
{
	D3DFVF = ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
	m_ppVB = NULL;
	m_ppIB = NULL;
	//m_FrameTextureID = NULL;
	m_pd3dDevice = D3DDevice;
	m_TextureList = new CTextureArray( m_pd3dDevice );
	m_FileName[0] = '\0';
	m_ModelDir[0] = '\0';
	m_TextureDir[0] = '\0';
	m_FilePath[0] =  '\0';
}
C3DTankModel::~C3DTankModel(void)
{
	Release();
	SAFE_DELETE( m_TextureList );
}
bool C3DTankModel::Create( char *FilePath )
{
	if( FilePath != m_FilePath )
		strcpy( m_FilePath, FilePath );
	if( m_ppVB || m_ppIB || m_Frames || m_pMaterial||
		m_FrameCount || m_MaterialCount || m_Frames ||
		m_pMaterial )
		return false;
	char szFile[MAX_PATH];
	strcpy( szFile, FilePath );
	if( szFile[1] != ':')
	{
		sprintf( szFile, "%s%s", m_ModelDir, GetFileName( FilePath ) );
	}
	strcpy( m_FileName , GetFileName( FilePath ) );
	return MyModel::Create( szFile );
}
void C3DTankModel::Release()
{
	ReleaseVBnIB();
	//SAFE_DELETE_ARRAY( m_FrameTextureID );
	SAFE_DELETE( m_TextureList );
	m_TextureList = new CTextureArray( m_pd3dDevice );
	MyModel::Release();
}
void C3DTankModel::SetDir( LPSTR ModelDir, LPSTR TextureDir )
{

/*	
	char *temp = GetFileName( ModelDir );
	if( strcmp( temp , "" ) )
		sprintf( m_ModelDir, "%s\\" , ModelDir );
	else
		strcpy( m_ModelDir, ModelDir );
	temp = GetFileName( TextureDir );
	if( strcmp( temp , "" ) )
		sprintf( m_TextureDir ,"%s\\" , TextureDir );
	else
		strcpy( m_TextureDir, TextureDir );
*/

	if( ModelDir )
	{
		if( strrchr( ModelDir, '\\' )[0] != '\0' )
			sprintf( m_ModelDir,"%s\\", ModelDir );
		else
			strcpy( m_ModelDir, ModelDir );
	}
	else  m_ModelDir[0] = '\0' ;
	if( TextureDir )
	{
		if( strrchr( TextureDir, '\\' )[0] != '\0' )
			sprintf( m_TextureDir,"%s\\", TextureDir );
		else
			strcpy( m_TextureDir, TextureDir );
	}
	else  m_TextureDir[0] = '\0' ;


}
DWORD C3DTankModel::GetTextureFromFile( LPSTR sFilePath, BYTE bKey )
{
	if( sFilePath[0] == '\0' ) return 0;
	char szFile[MAX_PATH];
	strcpy( szFile, sFilePath );
	if( szFile[1] != ':')
	{
		sprintf( szFile, "%s%s\\%s", m_TextureDir, GetFileTittle( m_FileName ), sFilePath );
	}
	//if( !m_FrameTextureID )
	//	if(!(m_FrameTextureID = new CTextureID[ m_FrameCount ]))
	//		return 0;

	BYTE key = bKey - 0x06;
	if( 0x00 <= key && key<= 0x03 )
	{
		DWORD ID = m_TextureList->Create( szFile );
		if(!ID)
			return 0;
		//*( &( m_FrameTextureID[ FrameIndex ].m_DiffuseMap ) + key ) = ID;
		return ID;
	}
	return 0;
}
HRESULT C3DTankModel::FrameToD3DTLVERTEX( D3DTLVERTEX *(&pOut), DWORD dwIndex )
{
	DWORD k;
	if( m_Frames[dwIndex].m_Vector.m_count >= 
		m_Frames[dwIndex].m_TextureVec.m_count )
	{
		DWORD count = m_Frames[dwIndex].m_Vector.m_count;
		D3DTLVERTEX *temp = new D3DTLVERTEX[ count ];
		if( !temp )
			return S_FALSE;
		for( k = 0; k < count; k++ )
		{
			memcpy( &(temp[k].Vector) , &(m_Frames[dwIndex].m_Vector.m_Vectors[k]) , sizeof( D3DVECTOR ) );
			memcpy( &(temp[k].Normal) , &(m_Frames[dwIndex].m_Normal.m_Vectors[k]) , sizeof( D3DVECTOR ) );
			//temp[k].tu = m_Frames[dwIndex].m_TextureVec.m_TextureVecs[k].tu;
			//temp[k].tv = m_Frames[dwIndex].m_TextureVec.m_TextureVecs[k].tv;


		}
		DWORD facecount = m_Frames[dwIndex].m_Face.m_count;
		DWORD i;
		for( k = 0; k < facecount; k++ )
		{
			
			for( i = 0; i < 3; i ++ )
				if( m_Frames[dwIndex].m_TextureVec.m_count )
				{
					temp[*(& ( m_Frames[dwIndex].m_Face.m_Faces[k]._0 ) + i )].tu = 
						m_Frames[dwIndex].m_TextureVec.m_TextureVecs
						[ *(& ( m_Frames[dwIndex].m_Face.m_TVFaces[k]._0 ) + i ) ].tu;

					temp[*(& ( m_Frames[dwIndex].m_Face.m_Faces[k]._0 ) + i )].tv = 
						m_Frames[dwIndex].m_TextureVec.m_TextureVecs
						[ *(& ( m_Frames[dwIndex].m_Face.m_TVFaces[k]._0 ) + i ) ].tv;
					
				}
				else
					temp[*(& ( m_Frames[dwIndex].m_Face.m_Faces[k]._0 ) + i )].tu =
					temp[*(& ( m_Frames[dwIndex].m_Face.m_Faces[k]._0 ) + i )].tv = 0.0f;
		}
		pOut = temp;
	}
	else
	{
		DWORD count = m_Frames[dwIndex].m_TextureVec.m_count;
		D3DTLVERTEX *temp = new D3DTLVERTEX[ count ];
		if( !temp )
			return S_FALSE;
		for( k = 0; k < count; k++ )
		{
			//memcpy( &(temp[k].Vector) , &(m_Frames[dwIndex].m_Vector.m_Vectors[k]) , sizeof( D3DVECTOR ) );
			//memcpy( &(temp[k].Normal) , &(m_Frames[dwIndex].m_Normal.m_Vectors[k]) , sizeof( D3DVECTOR ) );
			temp[k].tu = m_Frames[dwIndex].m_TextureVec.m_TextureVecs[k].tu;
			temp[k].tv = m_Frames[dwIndex].m_TextureVec.m_TextureVecs[k].tv;


		}
		DWORD facecount = m_Frames[dwIndex].m_Face.m_count;
		DWORD i;
		for( k = 0; k < facecount; k++ )
		{
			
			for( i = 0; i < 3; i ++ )
			{
					memcpy( &(temp[*(& ( m_Frames[dwIndex].m_Face.m_TVFaces[k]._0 ) + i )].Vector),
							&(m_Frames[dwIndex].m_Vector.m_Vectors
							[ *(& ( m_Frames[dwIndex].m_Face.m_Faces[k]._0 ) + i ) ]), sizeof(D3DVECTOR) );

					memcpy( &(temp[*(& ( m_Frames[dwIndex].m_Face.m_TVFaces[k]._0 ) + i )].Normal),
							&(m_Frames[dwIndex].m_Normal.m_Vectors
							[ *(& ( m_Frames[dwIndex].m_Face.m_Faces[k]._0 ) + i ) ]), sizeof(D3DVECTOR) );
			}
		}
		pOut = temp;
	}
	return S_OK;
}
HRESULT C3DTankModel::InitVBnIB()
{
	//InitVB
	if( m_ppVB || m_ppIB )
		return S_FALSE;
	m_ppVB = new LPDIRECT3DVERTEXBUFFER9[ m_FrameCount ];
	m_ppIB = new LPDIRECT3DINDEXBUFFER9[ m_FrameCount ];
	DWORD i;
	for( i=0; i < m_FrameCount; i++ )
	{
		DWORD count = ( m_Frames[i].m_Vector.m_count >= 
						m_Frames[i].m_TextureVec.m_count ?
						m_Frames[i].m_Vector.m_count:
						m_Frames[i].m_TextureVec.m_count );
		if( FAILED( m_pd3dDevice->CreateVertexBuffer( sizeof( D3DTLVERTEX )* count, 0 , D3DFVF ,
										  D3DPOOL_DEFAULT , &(m_ppVB[i]), NULL ) ) )
			return S_FALSE;
		D3DTLVERTEX *temp = NULL;
		if( FAILED ( FrameToD3DTLVERTEX( temp, i ) ) )
			return S_FALSE;
		LPVOID pData = NULL;
		m_ppVB[i]->Lock( 0, 0 , &pData , 0 );
		memcpy( pData, temp, sizeof(D3DTLVERTEX)*count );
		m_ppVB[i]->Unlock();
		SAFE_DELETE_ARRAY( temp );

		count = m_Frames[i].m_Face.m_count;
		
		if( FAILED( m_pd3dDevice->CreateIndexBuffer( sizeof( Face )* count, 0 , D3DFMT_INDEX32  ,
											  D3DPOOL_DEFAULT , &(m_ppIB[i]), NULL ) ) )
			  return S_FALSE;
	
		m_ppIB[i]->Lock( 0, 0, &pData, 0 );
		if( m_Frames[i].m_Vector.m_count >= 
		m_Frames[i].m_TextureVec.m_count )
			memcpy( pData, m_Frames[i].m_Face.m_Faces, sizeof( Face ) * count );
		else
			memcpy( pData, m_Frames[i].m_Face.m_TVFaces, sizeof( Face ) * count );
		m_ppIB[i]->Unlock();
	}

	return S_OK;
}
HRESULT C3DTankModel::ReleaseVBnIB()
{
	DWORD i;
	if( m_ppVB )
	{
		for( i = 0; i < m_FrameCount; i++ )
			SAFE_RELEASE( m_ppVB[i] );
		SAFE_DELETE_ARRAY( m_ppVB );
	}
	if( m_ppIB )
	{
		for( i = 0; i < m_FrameCount; i++ )
			SAFE_RELEASE( m_ppIB[i] );
		SAFE_DELETE_ARRAY( m_ppIB );
	}
	return S_OK;
}
HRESULT C3DTankModel::Render()
{
	if( !m_Frames )
		return S_FALSE;
	D3DXMATRIXA16 TempMatrix1(	1.0f,0,0,0,
								0,1.0f,0,0,
								0,0,1.0f,0,
								0,0,0,1.0f);
	D3DXMATRIXA16 TempMatrix2(	1.0f,0,0,0,
								0,1.0f,0,0,
								0,0,1.0f,0,
								0,0,0,1.0f);
	D3DMATERIAL9 DefaultMaterial = {	{ 0.5f, 0.5f, 0.5f, 1.0f },
										{ 0.5f, 0.5f, 0.5f, 1.0f },
										{ 1.0f, 1.0f, 1.0f, 1.0f },
										{ 0.0f, 0.0f, 0.0f, 0.0f },
										0.0f };
	m_pd3dDevice->GetTransform( D3DTS_WORLD , &TempMatrix1 );
	TempMatrix2 = TempMatrix1;
	DWORD i;
	if( m_ppVB && m_ppIB )
	{
		for( i = 0; i< m_FrameCount; i++)
		{

			TempMatrix2 = m_Frames[i].m_Transform * TempMatrix1 ;
			m_pd3dDevice->SetTransform( D3DTS_WORLD , &TempMatrix2 );

			m_pd3dDevice->SetFVF( D3DFVF );//设置FVF
			m_pd3dDevice->SetStreamSource( 0, m_ppVB[i], 0 , sizeof( D3DTLVERTEX ) );//载入顶点缓冲
			m_pd3dDevice->SetIndices( m_ppIB[i] );//载入面缓冲
			SetMaterial( i );

			DWORD veccount = ( m_Frames[i].m_Vector.m_count >= 
						m_Frames[i].m_TextureVec.m_count ?
						m_Frames[i].m_Vector.m_count:
						m_Frames[i].m_TextureVec.m_count );
			m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0, 0, 
												veccount, 0, 
												m_Frames[i].m_Face.m_count );


			
		}
		m_pd3dDevice->SetTransform( D3DTS_WORLD , &TempMatrix1 );
		return S_OK;
	}

	for( i = 0; i< m_FrameCount; i++)
	{
		TempMatrix2 = TempMatrix1 * m_Frames[i].m_Transform;
		m_pd3dDevice->SetTransform( D3DTS_WORLD , &TempMatrix2 );
		m_pd3dDevice->SetMaterial( (D3DMATERIAL9 *) &(m_pMaterial[*m_Frames[i].m_pMaterialID]) );
		DWORD facecount = m_Frames[i].m_Face.m_count;
		DWORD tempface[3] = { 0, 1, 2 };

		SetMaterial( i );

		for( DWORD j = 0; j < facecount; j++ )
		{
			m_pd3dDevice->SetFVF( D3DFVF );

			D3DTLVERTEX temp[3];
			memset( &temp, 0, sizeof( temp ) );
			for( DWORD k = 0; k < 3; k++ )
			{
				temp[k].Vector.x=m_Frames[i].m_Vector.m_Vectors[*(&m_Frames[i].m_Face.m_Faces[j]._0 + k)].x;
				temp[k].Vector.y=m_Frames[i].m_Vector.m_Vectors[*(&m_Frames[i].m_Face.m_Faces[j]._0 + k)].y;
				temp[k].Vector.z=m_Frames[i].m_Vector.m_Vectors[*(&m_Frames[i].m_Face.m_Faces[j]._0 + k)].z;
				
				temp[k].Normal.x=m_Frames[i].m_Normal.m_Vectors[*(&m_Frames[i].m_Face.m_Faces[j]._0 + k)].x;
				temp[k].Normal.y=m_Frames[i].m_Normal.m_Vectors[*(&m_Frames[i].m_Face.m_Faces[j]._0 + k)].y;
				temp[k].Normal.z=m_Frames[i].m_Normal.m_Vectors[*(&m_Frames[i].m_Face.m_Faces[j]._0 + k)].z;

				temp[k].tu=m_Frames[i].m_TextureVec.m_TextureVecs[*(&m_Frames[i].m_Face.m_TVFaces[j]._0 + k)].tu;
				temp[k].tv=m_Frames[i].m_TextureVec.m_TextureVecs[*(&m_Frames[i].m_Face.m_TVFaces[j]._0 + k)].tv;
			}
		
		m_pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST , 0, 3, 
											1, tempface , D3DFMT_INDEX32,
											temp, sizeof( D3DTLVERTEX ) );
		}
		
	}
	m_pd3dDevice->SetTransform( D3DTS_WORLD , &TempMatrix1 );
	return S_OK;
}
void C3DTankModel::Reset()
{
	if( m_FilePath[0] != '\0' )
	{
		Release();
		Create( m_FilePath );
	}
}