@echo off
setlocal enabledelayedexpansion enableextensions
set LIST=
for %%x in (..\source\ImgConverter\*.cpp) do set LIST=!LIST! %%x
set LIST=%LIST:~1%
xgettext -d ImgConverter -s --keyword=_ -p ../po -o ImgConverter.pot %LIST%
