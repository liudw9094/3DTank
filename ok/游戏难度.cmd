cls
@ECHO OFF
CLS
color 0a

GOTO MENU
:MENU
ECHO. 
ECHO.	===============游戏难度选择================
ECHO. 
ECHO.			1  入	门
ECHO.
ECHO.			2  简	单
ECHO.  
ECHO.			3  正	常
ECHO. 
ECHO.			4  困	难
ECHO.
ECHO.			5  取	消
ECHO.
ECHO.
ECHO.
CHOICE /C 12345 /N /M 请输入序号以选择难度：
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