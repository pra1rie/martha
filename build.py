#!/usr/bin/python3
import os

name = 'bin/martha'
libs = '-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf'

os.system(f'rm -f {name}')
os.system(f'gcc nara/*.c sauce/*.c -I nara {libs} -o {name}')
os.system('rm -rf *.o')
if os.path.exists(name):
    os.system(f'./{name}')

