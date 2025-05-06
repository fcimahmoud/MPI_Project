@echo off
setlocal

set INCLUDE_PATH="C:\Program Files (x86)\Microsoft SDKs\MPI\Include"
set LIB_PATH="C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64"
set LIBS=-lmsmpi
set SOURCES=main.cpp quick_search.cpp prime_finder.cpp bitonic_sort.cpp radix_sort.cpp sample_sort.cpp
set OUTPUT=a.exe

echo Compiling...
g++ %SOURCES% -I%INCLUDE_PATH% -L%LIB_PATH% %LIBS% -o %OUTPUT%
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build successful. Run with:
echo mpiexec -n 4 %OUTPUT%

endlocal