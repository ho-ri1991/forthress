PROGRAM = forthress
OBJS = lib.o forthress.o

ASM = nasm
ASMFLAGS = -felf64

LINKER = ld

.SUFFIXES: .asm .o

$(PROGRAM): $(OBJS)
	$(LINKER) -o $(PROGRAM) $^

.asm.o:
	$(ASM) $(ASMFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJS)

lib.o: lib.inc
forthress.o: macro.inc words.inc
