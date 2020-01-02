CC = i686-w64-mingw32-gcc
#CC = i586-mingw32msvc-gcc
CFLAGS = -Os -Wall
LINK = i586-mingw32msvc-ld
all: a.exe
a.exe: ping.o w32svc.o log.o
	#$(LINK) -o crt0.a $@ $^ -ldnsapi
	$(CC) -o $@ $^ -ldnsapi
clean:
	$(RM) *.o a.exe
