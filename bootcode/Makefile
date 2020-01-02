TOP = ..
PROJ = shellcode
include $(TOP)/config.mk
#CC = i686-w64-mingw32-gcc
#LINK = i686-w64-mingw32-ld


all: loader.bin
	@stat -c "Payload total %s bytes." loader.bin
loader.bin: loader.exe
	objcopy -O binary -j .text $^ $@
loader.exe: loader.o ldapi.o init.o
	$(LINK) $(LDFLAGS) -o $@ $^
init.o: init.asm
	$(AS) $(ASFLAGS) -o $@ $^
loader.o: loader.c
	$(CC) $(CFLAGS) -c -o $@ $^

disas: loader.bin
	ndisasm -b32 $^

test: test.c
	$(CC) -fno-stack-protector  -o $@.exe $^
test.c: loader.bin
	@xxd -i $^ > $@
	@echo "int main(){return ((int(*)())loader_bin)();}" >> $@
clean:
	$(RM) *.o loader.exe loader.bin 
	$(RM) test.c test.exe 
	$(RM) test_init.exe
test_init: init.o dynld.o
	$(CC) -o $@ $^ test.init.c
	$(PWD)/$@

include $(LIB)/lib.mk

.PHONY: all clean test_init
