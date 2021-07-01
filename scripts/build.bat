@echo off

set EXE_FILE_DIR=bin\win
set EXE_FILE_NAME=%EXE_FILE_DIR%\_test_qbPreAllocStack.exe

if NOT EXIST %EXE_FILE_DIR% ( mkdir %EXE_FILE_DIR% )
if EXIST %EXE_FILE_NAME% ( del %EXE_FILE_NAME% )
gcc -I. qbPreAllocStack.c test_qbPreAllocStack.c -o %EXE_FILE_NAME%
