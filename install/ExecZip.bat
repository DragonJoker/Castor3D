@echo OFF

set DATA_DIR=..\..\..\..\source\Core\Castor3D\Test\Data

for /f %%A in ('forfiles /s /p %DATA_DIR% /m *.zip') do (
	echo Testing %%~A
	CastorViewer /opengl %DATA_DIR%\%%~A
)