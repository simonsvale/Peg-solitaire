all:
	g++ -O -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows -static-libgcc -static-libstdc++