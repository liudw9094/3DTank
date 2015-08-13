#include "stdafx.h"
#include "util.h"



void PrintError( LPSTR Code, LPSTR FileName, DWORD Line, int ErrorCode )
{
		LPSTR lpBuffer;
		char errbuf[256];


		FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER
						| FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						ErrorCode,
						LANG_NEUTRAL,
						(LPTSTR)&lpBuffer,
						0,
						NULL );

		sprintf(errbuf, "\nThe following call failed at line %d in %s:\n\n"
				   "    %s\n\nReason: %s\n", Line, FileName, Code, lpBuffer);
		MessageBoxEx( errbuf );
}