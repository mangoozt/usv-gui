$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent;
$srcDir = Join-Path -Path $scriptDir -ChildPath ..\src;
Remove-Item build-win -Force -Recurse -ErrorAction Ignore
mkdir build-win;
cd build-win;
iwr https://sourceforge.net/projects/geographiclib/files/distrib/GeographicLib-1.50.1-win64.exe -OutFile geolib.exe -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::FireFox;
./geolib.exe /S | Out-Null;
Remove-Item ./geolib.exe | Out-Null;
cmake -DCMAKE_BUILD_TYPE=Release -GNinja $srcDir;
echo $(Get-WmiObject Win32_ComputerSystem).NumberOfProcessors
ninja -j $(Get-WmiObject Win32_ComputerSystem).NumberOfProcessors;