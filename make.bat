gcc -o beacon.exe main.c es2.c -mwindows -lpsapi -lws2_32 -static
strip --strip-all beacon.exe
