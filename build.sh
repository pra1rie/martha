#!/usr/bin/sh

# set -xe

NAME='bin/martha'
LIBS='-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf'

rm -f $NAME
gcc nara/*.c sauce/*.c -I nara $LIBS -o $NAME
rm -rf *.o

[ -f $NAME ] && ./$NAME

