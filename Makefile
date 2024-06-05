CC = clang
MUPDF_VERSION = 1.23.11
CFLAGS = -Wall -O3 \
	-I/usr/local/Cellar/mupdf/$(MUPDF_VERSION)/include \
	-L/usr/local/Cellar/mupdf/$(MUPDF_VERSION)/lib \
	-lmupdf-third -lmupdf

.PHONY: dylib example

dylib: libmuconvert.dylib

example: muconvert_example

muconvert_example: muconvert.c
	$(CC) $(CFLAGS) -o $@ $<

libmuconvert.dylib: muconvert.c
	$(CC) $(CFLAGS) -dynamiclib -o $@ $<
