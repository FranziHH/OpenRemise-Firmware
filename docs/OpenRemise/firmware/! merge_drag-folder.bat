@echo off
setlocal enabledelayedexpansion

REM Der Pfad des Ordners, den du daraufgezogen hast
set "targetDir=%~1"

if "%targetDir%"=="" (
    echo BITTE EINEN ORDNER AUF DIESE DATEI ZIEHEN!
    pause
    exit /b
)

echo Verarbeite Ordner: "%targetDir%"
cd /d "%targetDir%"

REM Wir bauen den Befehl zusammen. 
REM Wir nutzen die Pfade aus deinem Original-Script.

esptool --chip esp32s3 merge-bin ^
  -o Firmware_Full.bin ^
  --flash-mode dout ^
  --flash-freq 80m ^
  --flash-size 32MB ^
  0x0 bootloader/bootloader.bin ^
  0x8000 partition_table/partition-table.bin ^
  0x9000 ota_data_initial.bin ^
  0x10000 Firmware.bin ^
  0xc10000 nvs.bin

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ERFOLG! Die Datei "Firmware_Full.bin" wurde im Ordner erstellt.
) else (
    echo.
    echo FEHLER! Irgendwas hat nicht geklappt. Stimmen die Unterordner?
)

echo.
echo press any key
pause > nul
