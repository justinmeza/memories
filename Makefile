all:
	gcc memories.c -lGL -lGLU -lglut -D_POSIX_C_SOURCE=199309L -o memories
	./memories
