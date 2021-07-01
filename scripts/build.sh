#!/bin/bash

if [ ! -d bin ] ; then
  echo "ERROR: bin/ directory doesn't exist. Are you in the project root directory ?"
  echo exit 1
fi

gcc -I. qbPreAllocStack.c test_qbPreAllocStack.c -o bin/_qbPreAllocStack.x
