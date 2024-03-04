rm static/beacon
gcc main.c -static -o static/beacon
strip --strip-all static/beacon
