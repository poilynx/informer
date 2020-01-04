ldapi.o: $(LIB)/ldapi.asm
	$(AS) $(ASFLAGS) -o $@ $^
ping.o: $(LIB)/proto/ping.c
	$(CC) -c $(CFLAGS) -o $@ $^
