CC = clang
MUPDF_VERSION = 1.23.11
CFLAGS = -Wall -O3 \
  -I/usr/local/Cellar/mupdf/$(MUPDF_VERSION)/include \
  -L/usr/local/Cellar/mupdf/$(MUPDF_VERSION)/lib \
  -lmupdf-third -lmupdf

muconvert_stdout: muconvert_stdout.c
	$(CC) $(CFLAGS) -o $@ $<
