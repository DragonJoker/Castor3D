@echo off
setlocal enabledelayedexpansion enableextensions
set LIST=
for %%x in (..\GuiCommon\Src\*.cpp) do set LIST=!LIST! %%x
for %%x in (..\CastorViewer\Src\*.cpp) do set LIST=!LIST! %%x
xgettext -d CastorViewer -s --keyword=_ -p .\po\ -o CastorViewer.pot %LIST%
