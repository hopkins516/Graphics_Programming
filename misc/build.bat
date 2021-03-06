@echo off
set CompilerFlags=-MDd -nologo -Gm- -GR- -EHsc -Od -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DSLOW=1 -DOPENGL=1 -FC -Zi -fp:fast  
set LinkerFlags= -incremental:no -opt:ref  user32.lib Gdi32.lib winmm.lib shell32.lib 
IF NOT EXIST ..\build mkdir ..\build
IF NOT EXIST ..\build\assimp-vc140-mt.dll (
	copy ..\libs\assimp-vc140-mt.dll ..\build\
)
IF NOT EXIST ..\build\portaudio_x64.dll (
	copy ..\libs\portaudio\portaudio_x64.dll ..\build\
)
pushd ..\build
cl %CompilerFlags% -I "..\libs" -DENGINE_EXPORTS=1  ..\engine\engine.cpp -LD -link %LinkerFlags% ..\libs\glfw3.lib ..\libs\assimp-vc140-mt.lib opengl32.lib ..\libs\portaudio\portaudio_x64.lib -incremental:no  
cl %CompilerFlags% -I "..\libs" -I "..\engine" ..\test_game\main.cpp -link %LinkerFlags% engine.lib ..\libs\portaudio\portaudio_x64.lib
popd