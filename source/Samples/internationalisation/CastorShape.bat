@echo off
setlocal enabledelayedexpansion enableextensions
set LIST=
for %%x in (..\GuiCommon\Src\*.cpp) do set LIST=!LIST! %%x
for %%x in (..\CastorShape\Src\*.cpp) do set LIST=!LIST! %%x
xgettext -d CastorShape -s --keyword=_ -p .\po\ -o CastorShape.pot %LIST%
