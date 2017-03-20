cd pk7
C:\stuff\utils\7z\7za.exe a %2 ..\Lithium.%1 .\* -xr!*.gitignore -xr!*.bat -xr!*.dbs -xr!*.wad.b*

cd ..\pk7_pausemenu
C:\stuff\utils\7z\7za.exe a %2 ..\Lithium_PauseInMenu.%1 .\*
