#ifndef NULL
	#define NULL 0
#endif
#define FILE_FREME 0x01
#define FILE_VECTORS 0x02
#define FILE_FACES 0x03
#define FILE_MATRIX 0x04
#define FILE_MATERIAL 0x05

//0x00 NULL, 0x01 Frame, 0x02 Vectors,0x03 Faces,0x04 Matrix,0x05 Material ID

//0x06 Undefined 0x07 Undefined,0x08 Undefined, 0x09 Undefined
/*typedef struct vec
{
	float x;
	float y;
	float z;
} VECTOR,D3DVECTOR,*LPVECTOR,*LPD3DVECTOR;

typedef struct tr{
	float _00,_01,_02,_03;
	float _10,_11,_12,_13;
	float _20,_21,_22,_23;
	float _30,_31,_32,_33;
}Trans,D3DMATRIX,*LPTRANS,*LPD3DMATRIX;
typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE,D3DXCOLOR,*LPD3DXCOLOR;*/
typedef struct _MYMATERIAL {
    D3DXCOLOR Diffuse;
    D3DXCOLOR Ambient;
    D3DXCOLOR Specular;
    D3DXCOLOR Emissive;
    float Power;
	DWORD	m_DiffuseMap;
	DWORD	m_SpecularMap;
	DWORD	m_BumpMap;
	DWORD	m_DisplacementMap;
	_MYMATERIAL()
	{
		Diffuse = Ambient = D3DCOLOR_RGBA( 177, 177, 177, 255 );
		Specular = D3DCOLOR_RGBA( 255, 255, 255, 255 );
		Emissive = D3DCOLOR_RGBA( 0, 0, 0, 255 );
		Power = 0.0f;
		m_DiffuseMap = m_SpecularMap = m_BumpMap = m_DisplacementMap = 0L;
	}
} DMODMATERIAL;

typedef struct fa{
	DWORD _0;
	DWORD _1;
	DWORD _2;
}Face,*LPFACE;
typedef struct _TextureVec
{
	float tu,tv,tw;
}TextureVec,*LPTEXTUREVEC;
class MyTextureVec
{
public:
	MyTextureVec(){ m_count = 0; m_TextureVecs = NULL;};
	~MyTextureVec()
	{
		if(m_TextureVecs)
			delete[] m_TextureVecs;
	};
	DWORD m_count;
	LPTEXTUREVEC m_TextureVecs;
	void Create( DWORD Count );
};
class MyVector
{
public:
	MyVector(){	m_count = 0; m_Vectors = NULL;};
	~MyVector()
	{
		if(m_Vectors)
			delete[] m_Vectors;
	};
	DWORD m_count;
	LPD3DVECTOR m_Vectors;
	void Create( DWORD Count );
};
class MyFaces
{
public:
	MyFaces(){ m_count = 0; m_Faces = NULL; m_TVFaces = NULL; };
	~MyFaces()
	{ 
		if(m_Faces)
			delete[] m_Faces;
		if(m_TVFaces)
			delete[] m_TVFaces;
	};
	DWORD m_count;
	LPFACE m_Faces;
	LPFACE m_TVFaces;
	void Create( DWORD Count );

};

class Myframe
{
public:
	Myframe(){ m_pMaterialID = NULL; m_MaterialCount = 0; };
	~Myframe()
	{
		if( m_pMaterialID )
			delete [] m_pMaterialID;
	};
	char name[256];
	MyVector m_Vector;
	MyVector m_Normal;
	MyFaces m_Face;
	D3DXMATRIX	m_Transform;
	DWORD m_MaterialCount;
	DWORD *m_pMaterialID;
	MyTextureVec m_TextureVec;

};

class MyModel
{
public:
	MyModel(){ m_FrameCount = 0 ; m_Frames = NULL ; m_pMaterial = NULL; m_MaterialCount = 0; };
	~MyModel(){ Release(); };
	Myframe *m_Frames;
	DWORD m_FrameCount;
	DWORD m_MaterialCount;
	DMODMATERIAL *m_pMaterial;
	virtual bool Create( char * FilePath );
	virtual bool GetMaterial( FILE *hFile );
	virtual bool GetNext( FILE *hFile , DWORD temp = (DWORD)-1 );
	virtual bool GetTexture( FILE *hFile, DWORD MaterialIndex );
	virtual DWORD GetTextureFromFile( LPSTR sFilePath, BYTE bKey ){ return true; }
	virtual void Release();
};