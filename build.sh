#!/usr/bin/sh

set -e

NAME='martha'
LIBS='-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf'

rm -f bin/$NAME
gcc sauce/*.c nara/*.c -I nara $LIBS -o bin/$NAME
rm -rf *.o

[ -f bin/$NAME ]; { cd bin; ./$NAME; }

