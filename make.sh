rm static/beacon
gcc main.c -O5 -static -o static/beacon
strip --strip-all static/beacon
