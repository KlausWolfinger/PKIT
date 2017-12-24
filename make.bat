echo "MLPKI Maker --- V2"
REM @echo off
START "" "c:\Programme\7-Zip\7z.exe" a -r mlpki.zip pkiconf host sbin pki.sh -p0815
REM echo off
set /p id=Fertig [ENTER] 
exit;