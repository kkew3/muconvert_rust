/* A simple working example of muconvert */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "muconvert.h"

// Allocate at most 200 MiB buffer for the pdf.
#define SIZE_UPPER_LIMIT 209715200

long get_file_size(const char *filename) {
    FILE *infile = fopen(filename, "r");
    if (infile == NULL) {
        return -1;
    }
    fseek(infile, 0L, SEEK_END);
    long size = ftell(infile);
    fclose(infile);
    return size;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <pdf-file>\n", argv[0]);
        return 1;
    }
    // Use the file size as a heuristics on how many bytes to allocate pdf
    // buffer.
    long lsize = get_file_size(argv[1]);
    if (lsize < 0) {
        fprintf(stderr, "Err: failed to read file: %s\n", argv[1]);
        return 1;
    }
    if (lsize == 0) {
        return 0;
    }
    size_t size = (size_t) lsize;
    // Extract text here.
    unsigned char *buf = (unsigned char *) malloc(size * sizeof(unsigned char));
    if (buf == NULL) {
        fprintf(stderr, "Fatal: failed to allocate buffer\n");
        return 255;
    }
    memset(buf, 0, size * sizeof(unsigned char));
    int retval = pdftotext(argv[1], 0, buf, &size);
    if (retval == 1) {
        fprintf(stderr, "Err: failed to extract pdf text\n");
        free(buf);
        return 1;
    }
    if (retval == 2) {
        if (size > SIZE_UPPER_LIMIT) {
            fprintf(stderr, "Err: required buffer size exceeds upper limit\n");
            free(buf);
            return 1;
        }
        buf = (unsigned char *) realloc(buf, size * sizeof(unsigned char));
        if (buf == NULL) {
            fprintf(stderr, "Fatal: failed to allocate buffer\n");
            return 255;
        }
        memset(buf, 0, size * sizeof(unsigned char));
        // Retry extracting text here.
        retval = pdftotext(argv[1], 0, buf, &size);
        if (retval != 0) {
            fprintf(stderr, "Err: failed to extract pdf text\n");
            free(buf);
            return 1;
        }
    }
    printf("%s", buf);
    free(buf);
    return 0;
}
