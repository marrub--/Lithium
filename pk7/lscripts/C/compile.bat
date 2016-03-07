@echo off

setlocal

set LFLAGS=--bc-target=ZDoom --bc-zdacs-init-delay
set CFLAGS=--bc-target=ZDoom -iinc -Dnull=NULL

if not exist bin/libc.ir (
   C:\gdcc\gdcc-makelib.exe --bc-target=ZDoom -c libc -o bin/libc.ir
)

if not exist bin/libGDCC.ir (
   C:\gdcc\gdcc-makelib.exe --bc-target=ZDoom -c libGDCC -o bin/libGDCC.ir
)

if not exist ../../acs/lithlib.bin (
   C:\gdcc\gdcc-ld.exe %LFLAGS% --func-minimum ScriptI 400 --alloc-min Sta "" 300000000 ^
      bin/libGDCC.ir ^
      bin/libc.ir ^
      -o ..\..\acs\lithlib.bin
)

C:\gdcc\gdcc-cc.exe %CFLAGS% -c src/lith_list.c -o bin/lith_list.ir
C:\gdcc\gdcc-ld.exe %LFLAGS% --func-minimum ScriptI 300 --alloc-min Sta "" 1000000000 ^
   -llithlib ^
   bin/lith_list.ir ^
   -o ..\..\acs\lithlist.bin

C:\gdcc\gdcc-cc.exe %CFLAGS% -c src/lith_common.c -o bin/lith_common.ir
C:\gdcc\gdcc-cc.exe %CFLAGS% -c src/lith.c -o bin/lith.ir
C:\gdcc\gdcc-cc.exe %CFLAGS% -c src/lith_hud.c -o bin/lith_hud.ir
C:\gdcc\gdcc-cc.exe %CFLAGS% -c src/lith_decorate.c -o bin/lith_decorate.ir
C:\gdcc\gdcc-ld.exe %LFLAGS% --func-minimum ScriptI 200 --alloc-min Sta "" 3000000000 ^
   -llithlib ^
   -llithlist ^
   bin/lith_common.ir ^
   bin/lith.ir ^
   bin/lith_hud.ir ^
   bin/lith_decorate.ir ^
   -o ..\..\acs\lithmain.bin

endlocal
