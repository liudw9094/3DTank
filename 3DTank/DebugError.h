#ifdef _DEBUG
	#define WINAPITRY(p) { if(!(p)){ PrintError( #p , __FILE__ , __LINE__ , GetLastError()}}
#else
	#define WINAPITRY(p) (p)
#endif
void PrintError( LPSTR Code, LPSTR FileName, DWORD Line, int ErrorCode );
