#!/usr/bin/sh

# set -xe

NAME='bin/martha'
LIBS='-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf'

# lmao agian
cd nara/
./build.sh
cd ..

rm -f $NAME
gcc sauce/*.c -I nara/include -Lnara -lnara $LIBS -o $NAME
rm -rf *.o

[ -f $NAME ] && ./$NAME

