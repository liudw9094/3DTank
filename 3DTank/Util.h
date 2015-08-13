//#include<new.h>

#define SAFE_RELEASE(p)			{if(p){ (p)->Release(); (p) = NULL; }}
#define SAFE_DELETE(p)			{if(p){ delete (p); (p) = NULL; }}
#define SAFE_DELETE_ARRAY(p)	{if(p){ delete [] (p); (p) = NULL; }}

#define SAFE_CALL_RESULT(p)		{ int r = (p); if(r)	return r;}
#define SAFE_CALL_BOOL(p)		SAFE_CALL_RESULT(!(p))
#define TOLONG(x) ((x)<0 ? (long)((x)-0.5) : (long)((x) + 0.5 ) )
#define MAX(x,y) ((x)>(y)?(x):(y))

void MessageBoxEx( LPSTR str, bool err = true );
float __fastcall usTimeCounter( bool Start = false );
BOOL SetDisplaySize( HWND hWnd , int width , int height );
char *GetFileName( LPSTR sPath );
char *GetFileTittle( LPSTR sPath );
void DoEvents();
void SetLogPath( const char* sPath );
void WriteLog( const char* sLog );
void ClearLog();
class CCriticalSection//临界保护类
{
private:
	static bool m_bCSInited;
	static CRITICAL_SECTION	m_CS;
public:
	CCriticalSection();
	~CCriticalSection();
public:
	static void Init( DWORD dwSpinCount = 0 );
	static void Lock();
	static void Unlock();
	static void Release();
	static inline const bool IsInited(){ return m_bCSInited; };

};


class CLock
{
private:
	typedef CCriticalSection m_tCS;
public:
	inline CLock(){ if(m_tCS::IsInited()) m_tCS::Init(); m_tCS::Lock(); };
	inline ~CLock(){ m_tCS::Unlock(); };
};


//链表类
template < typename type >
class UTIL_ARRAY
{
public:
	UTIL_ARRAY(){ Unit = NULL; count = 0; };
	virtual ~UTIL_ARRAY(){ Release(); };
	DWORD Append( type *list, DWORD num = 1 );

	bool Delete( DWORD index_start, DWORD num = 1 );
	virtual void Release( bool bAutoRelease = false );
	type *GetUnit( DWORD index )
	{
		if( Unit && index < count )
			return Unit[index];
		else
			return NULL;
	};
	type **GetpUnit( DWORD index )
	{
		if( Unit && index < count )
			return &(Unit[index]);
		else
			return NULL;
	};
	DWORD GetCount(){ return count; };
	inline type* operator [] ( DWORD index ){ return GetUnit( index ); };
protected:
	type **Unit;
	DWORD count;


};
template < typename type >
DWORD UTIL_ARRAY<type>::Append( type *list, DWORD num )
{
	DWORD result = 0;
	if( num == 0 ) return 0;
	type **temp;
	temp = new type*[ num + count ];
	memcpy( temp, Unit, sizeof(type**)*count );
	type *temp1 = list;
	for( DWORD i = 0; i < num; i++ )
		temp[ count + i ] = &(temp1[i]);
	type **temp2 = Unit;
	Unit = temp;
	SAFE_DELETE_ARRAY(temp2);
	result = count + 1;
	count += num;
	return result;

}
template < typename type >
bool UTIL_ARRAY<type>::Delete( DWORD index_start, DWORD num = 1 )
{
	if( num == 0 || index_start + num > count ) return false;
	type **temp;
	temp = new type*[ count - num ];
	for( DWORD i = 0; i < index_start; i++ )
		temp[i] = Unit[i];
	for( DWORD i = index_start + num - 1; i < count; i++ )
		temp[ i - num ] = Unit[i];
	for ( DWORD i = 0; i < num; i++ )
		SAFE_DELETE( Unit[ i + index_start - 1 ] );
	type **temp2 = Unit;
	Unit = temp;
	SAFE_DELETE_ARRAY(temp2);
	count -= num;
	return true;
}
template < typename type >
void UTIL_ARRAY<type>::Release( bool bAutoRelease = false )
{
	if( !count || !Unit )
		return;
	if( bAutoRelease )
	{
		for( DWORD i = 0; i < count; i++ )
			SAFE_DELETE( Unit[i] );
	}
	SAFE_DELETE_ARRAY( Unit );
	count = 0;
}

