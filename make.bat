del static/beacon
gcc -O5 -o static/beacon.exe main.c es2.c -mwindows -lpsapi -lws2_32 -static
strip --strip-all static/beacon.exe
