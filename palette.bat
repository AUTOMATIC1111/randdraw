@echo off

set COMMAND=%~dp0\cmake-build-release\extract-colors.exe
set FILENAME=%1
set FILE=%FILENAME%
set FILE=%FILE:.png=%
set FILE=%FILE:.jpg=%
set FILE=%FILE:.gif=%

%COMMAND% -q -c 3 %FILENAME% %FILE%.palette-3.png -m %FILE%.map-3.png
%COMMAND% -q -c 4 %FILENAME% %FILE%.palette-4.png -m %FILE%.map-4.png
%COMMAND% -q -c 5 %FILENAME% %FILE%.palette-5.png -m %FILE%.map-5.png
%COMMAND% -q -c 6 %FILENAME% %FILE%.palette-6.png -m %FILE%.map-6.png

montage %FILE%.palette-3.png %FILE%.palette-4.png %FILE%.palette-5.png %FILE%.palette-6.png -geometry +16+16 -tile 4x1 %FILE%.palettes.png

montage ( %FILE%.map-3.png -resize 140x ) ( %FILE%.map-4.png -resize 140x ) ( %FILE%.map-5.png -resize 140x ) ( %FILE%.map-6.png -resize 140x ) -geometry +0+0 -tile 4x1 %FILE%.maps.png

montage ( %FILENAME% -resize 560x ) %FILE%.palettes.png %FILE%.maps.png -geometry +0+0 -tile 1x -adjoin %FILE%.palette.png
del %FILE%.palette-3.png %FILE%.palette-4.png %FILE%.palette-5.png %FILE%.palette-6.png %FILE%.palettes.png %FILE%.map-3.png %FILE%.map-4.png %FILE%.map-5.png %FILE%.map-6.png  %FILE%.maps.png
