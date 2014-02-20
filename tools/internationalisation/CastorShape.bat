@echo off
setlocal enabledelayedexpansion enableextensions
set LIST=
for %%x in (..\source\GuiCommon\*.cpp) do set LIST=!LIST! %%x
for %%x in (..\source\CastorShape\*.cpp) do set LIST=!LIST! %%x
for %%x in (..\source\CastorShape\geometry\*.cpp) do set LIST=!LIST! %%x
for %%x in (..\source\CastorShape\main\*.cpp) do set LIST=!LIST! %%x
for %%x in (..\source\CastorShape\material\*.cpp) do set LIST=!LIST! %%x
xgettext -d CastorShape -s --keyword=_ -p ../po -o CastorShape.pot %LIST%
