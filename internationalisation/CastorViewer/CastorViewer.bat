@echo off
setlocal enabledelayedexpansion enableextensions
set LIST=
for %%x in (..\..\tools\GuiCommon\*.cpp) do set LIST=!LIST! %%x
for %%x in (..\..\tools\CastorViewer\*.cpp) do set LIST=!LIST! %%x
xgettext -d CastorViewer -s --keyword=_ -p .\po\ -o CastorViewer.pot %LIST%
