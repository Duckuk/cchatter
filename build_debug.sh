#!/usr/bin/env bash

mkdir -p bin/debug

clang -g -O0 client/main.c -o bin/debug/client
clang -g -O0 server/main.c -o bin/debug/server