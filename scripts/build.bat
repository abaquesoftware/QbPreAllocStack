@echo off

set EXE_FILE_DIR=bin
set EXE_FILE_NAME=%EXE_FILE_DIR%\_test_qbPreAllocStack.exe

if NOT EXIST %EXE_FILE_DIR% ( echo "ERROR: bin/ doesn't exist - Are you in the project root directory ?" && exit 1 )
gcc -I. qbPreAllocStack.c test_qbPreAllocStack.c -o %EXE_FILE_NAME%
