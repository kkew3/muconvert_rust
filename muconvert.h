#ifndef _MUCONVERT_H_
#define _MUCONVERT_H_

#include <mupdf/fitz.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* input options */
typedef struct input_opts
{
	int alphabits;
	float layout_w;
	float layout_h;
	float layout_em;
	int layout_use_doc_css;
} input_opts;

/* output options */
typedef struct output_opts
{
	int dehyphenate;
} output_opts;

typedef struct run_param
{
	fz_context *ctx;
	fz_document *doc;
	fz_document_writer *out;
	fz_box_type page_box;
	int count;
} run_param;

/*
 * Extract text from pdf named `filename`.
 *
 * Arguments:
 *   filename      -- the path to the pdf
 *   dehyphenate   -- nonzero to enable 'dehyphenate' option in mupdf
 *   data          -- a large enough buffer to receive data from mupdf
 *   len           -- as input, the size of `data`; as output, the size of the
 *                    pdf data from mupdf
 *
 * Return:
 *   retcode       -- 0 for success, 1 for error, 2 for buffer not big enough
 */
int pdftotext(const char *filename, int dehyphenate, unsigned char *data, size_t *len);

#endif
