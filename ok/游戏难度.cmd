cls
@ECHO OFF
CLS
color 0a

GOTO MENU
:MENU
ECHO. 
ECHO.	===============��Ϸ�Ѷ�ѡ��================
ECHO. 
ECHO.			1  ��	��
ECHO.
ECHO.			2  ��	��
ECHO.  
ECHO.			3  ��	��
ECHO. 
ECHO.			4  ��	��
ECHO.
ECHO.			5  ȡ	��
ECHO.
ECHO.
ECHO.
CHOICE /C 12345 /N /M �����������ѡ���Ѷȣ�
IF ERRORLEVEL 1 IF NOT ERRORLEVEL 2 GOTO BEGINER
IF ERRORLEVEL 2 IF NOT ERRORLEVEL 3 GOTO EASY
IF ERRORLEVEL 3 IF NOT ERRORLEVEL 4 GOTO NORMAL
IF ERRORLEVEL 4 IF NOT ERRORLEVEL 5 GOTO HARD
IF ERRORLEVEL 5 GOTO END
PAUSE
:BEGINER
copy /Y .\beginer.dat .\config.ini
GOTO END
:EASY
copy /Y .\easy.dat .\config.ini
GOTO END
:NORMAL
copy /Y .\normal.dat .\config.ini
GOTO END
:HARD
copy /Y .\hard.dat .\config.ini
GOTO END
:END