#include <mupdf/fitz.h>

#include <stdio.h>
#include <stdlib.h>

/* input options */
static const char *password = "";
static int alphabits = 8;
static float layout_w = FZ_DEFAULT_LAYOUT_W;
static float layout_h = FZ_DEFAULT_LAYOUT_H;
static float layout_em = FZ_DEFAULT_LAYOUT_EM;
static char *layout_css = NULL;
static int layout_use_doc_css = 1;

/* output options */
static const char *format = "text";
static const char *options = "";

static fz_context *ctx;
static fz_document *doc;
static fz_document_writer *out;
static fz_box_type page_box = FZ_CROP_BOX;
static int count;

static void runpage(int number)
{
	fz_rect box;
	fz_page *page;
	fz_device *dev = NULL;
	fz_matrix ctm;

	page = fz_load_page(ctx, doc, number - 1);

	fz_var(dev);

	fz_try(ctx)
	{
		box = fz_bound_page_box(ctx, page, page_box);

		// Realign page box on 0,0
		ctm = fz_translate(-box.x0, -box.y0);
		box = fz_transform_rect(box, ctm);

		dev = fz_begin_page(ctx, out, box);
		fz_run_page(ctx, page, dev, ctm, NULL);
		fz_end_page(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_page(ctx, page);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);
}

static void runrange(const char *range)
{
	int start, end, i;

	while ((range = fz_parse_page_range(ctx, range, &start, &end, count)))
	{
		if (start < end)
			for (i = start; i <= end; ++i)
				runpage(i);
		else
			for (i = start; i >= end; --i)
				runpage(i);
	}
}

int main(int argc, char **argv)
{
    int i;
	int retval = EXIT_SUCCESS;

    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx) {
        fprintf(stderr, "cannot create mupdf context\n");
        return EXIT_FAILURE;
    }

    fz_try(ctx)
        fz_register_document_handlers(ctx);
    fz_catch(ctx)
    {
        // fz_report_error(ctx);
        fprintf(stderr, "cannot register document handlers\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
    }

    fz_set_aa_level(ctx, alphabits);

    if (layout_css)
	{
		fz_buffer *buf = fz_read_file(ctx, layout_css);
		fz_set_user_css(ctx, fz_string_from_buffer(ctx, buf));
		fz_drop_buffer(ctx, buf);
	}

	fz_set_use_document_css(ctx, layout_use_doc_css);

    fz_try(ctx)
        out = fz_new_text_writer_with_output(ctx, format, fz_stdout(ctx), options);
    fz_catch(ctx)
    {
        // fz_report_error(ctx);
		fprintf(stderr, "cannot create document\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
    }

    fz_var(doc);
	fz_try(ctx)
	{
		for (i = 1; i < argc; ++i)
		{
			doc = fz_open_document(ctx, argv[i]);
			if (fz_needs_password(ctx, doc))
				if (!fz_authenticate_password(ctx, doc, password))
				{
					// fz_throw(ctx, FZ_ERROR_ARGUMENT, "cannot authenticate password: %s", argv[i]);
					fprintf(stderr, "cannot authenticate password\n");
				}
			fz_layout_document(ctx, doc, layout_w, layout_h, layout_em);
			count = fz_count_pages(ctx, doc);

			if (i+1 < argc && fz_is_page_range(ctx, argv[i+1]))
				runrange(argv[++i]);
			else
				runrange("1-N");

			fz_drop_document(ctx, doc);
			doc = NULL;
		}
		fz_close_document_writer(ctx, out);
	}
	fz_always(ctx)
	{
		fz_drop_document(ctx, doc);
		fz_drop_document_writer(ctx, out);
	}
	fz_catch(ctx)
	{
		// fz_report_error(ctx);
		retval = EXIT_FAILURE;
	}

	fz_drop_context(ctx);
    return retval;
}
