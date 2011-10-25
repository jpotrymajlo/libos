@echo off
:: SLIM jest AWK-owym przekodownikiem pomi/edzy r/o/znymi systemami
:: reprezentowania polskich liter.

:: Poniedzia’ek, 12 lutego 1996 r., wersja 0.55

if "%3"=="" goto BYK
if "%4"=="" goto NAEKRAN
if "%3"=="%4" goto NAEKRAN

:NAPLIK
gawk -fslim.awk -vTBL=slim.tbl -vS=%1 -vR=%2 -vOFI=%4 %3
cp %4 %3
rm %4
goto GOTOWE

:NAEKRAN
gawk -fslim.awk -vTBL=slim.tbl -vS=%1 -vR=%2 %3
goto GOTOWE

:BYK
echo Musz/a by/c co najmniej trzy parametry:
echo    1. znak okre/slaj/acy kodowanie pliku /xr/od/lowego
echo    2. znak okre/slaj/acy kodowanie pliku wynikowego
echo    3. nazwa pliku /xr/od/lowego
echo i ewentualnie
echo    4. nazwa pliku wynikowego
echo Wi/ecej szczeg/o/l/ow mo/zna znale/x/c w pliku SLIM.INF.

:GOTOWE
