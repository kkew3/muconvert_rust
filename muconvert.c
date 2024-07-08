#include "muconvert.h"

void runpage(run_param *param, int number)
{
	fz_rect box;
	fz_page *page;
	fz_device *dev = NULL;
	fz_matrix ctm;

	page = fz_load_page(param->ctx, param->doc, number - 1);

	fz_var(dev);

	fz_try(param->ctx)
	{
		box = fz_bound_page_box(param->ctx, page, param->page_box);

		// Realign page box on 0,0
		ctm = fz_translate(-box.x0, -box.y0);
		box = fz_transform_rect(box, ctm);

		dev = fz_begin_page(param->ctx, param->out, box);
		fz_run_page(param->ctx, page, dev, ctm, NULL);
		fz_end_page(param->ctx, param->out);
	}
	fz_always(param->ctx)
	{
		fz_drop_page(param->ctx, page);
	}
	fz_catch(param->ctx)
		fz_rethrow(param->ctx);
}

void runrange(run_param *param, const char *range)
{
	int start, end, i;

	while ((range = fz_parse_page_range(param->ctx, range, &start, &end, param->count)))
	{
		if (start < end)
			for (i = start; i <= end; ++i)
				runpage(param, i);
		else
			for (i = start; i >= end; --i)
				runpage(param, i);
	}
}

int pdftotext(const char *filename, int dehyphenate, unsigned char *data, size_t *len)
{
	int retval = 0;
	run_param param;
	input_opts in_opts;
	in_opts.alphabits = 8;
	in_opts.layout_em = FZ_DEFAULT_LAYOUT_EM;
	in_opts.layout_h = FZ_DEFAULT_LAYOUT_H;
	in_opts.layout_w = FZ_DEFAULT_LAYOUT_W;
	in_opts.layout_use_doc_css = 1;
	output_opts out_opts;
	out_opts.dehyphenate = dehyphenate;
	unsigned char *buf_ptr;
	size_t buf_size;

    param.ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!param.ctx) {
        fprintf(stderr, "cannot create mupdf context\n");
        return 1;
    }

    fz_try(param.ctx)
        fz_register_document_handlers(param.ctx);
    fz_catch(param.ctx)
    {
        fz_log_error_printf(param.ctx, "cannot register document handlers\n");
		fz_drop_context(param.ctx);
		return 1;
    }

    fz_set_aa_level(param.ctx, in_opts.alphabits);

	fz_set_use_document_css(param.ctx, in_opts.layout_use_doc_css);
	fz_buffer *buf = fz_new_buffer(param.ctx, *len);

    fz_try(param.ctx)
        param.out = fz_new_text_writer_with_output(param.ctx, "text", fz_new_output_with_buffer(param.ctx, buf), out_opts.dehyphenate ? "dehyphenate" : "");
    fz_catch(param.ctx)
    {
        fz_log_error_printf(param.ctx, "cannot create document\n");
		fz_drop_buffer(param.ctx, buf);
		fz_drop_context(param.ctx);
		return 1;
    }

    fz_var(param.doc);
	fz_try(param.ctx)
	{
		param.doc = fz_open_document(param.ctx, filename);
		if (fz_needs_password(param.ctx, param.doc))
			if (!fz_authenticate_password(param.ctx, param.doc, ""))
			{
				fz_throw(param.ctx, 1, "cannot authenticate password: %s", filename);
				fz_log_error_printf(param.ctx, "cannot authenticate password\n");
			}
		fz_layout_document(param.ctx, param.doc, in_opts.layout_w, in_opts.layout_h, in_opts.layout_em);
		param.count = fz_count_pages(param.ctx, param.doc);

		runrange(&param, "1-N");

		fz_drop_document(param.ctx, param.doc);
		param.doc = NULL;

		fz_close_document_writer(param.ctx, param.out);
		buf_size = fz_buffer_storage(param.ctx, buf, &buf_ptr);
		if (buf_size >= *len)
		{
			retval = 2;
		}
		else
		{
			memcpy(data, buf_ptr, buf_size);
		}
		*len = buf_size;
	}
	fz_always(param.ctx)
	{
		fz_drop_document(param.ctx, param.doc);
		fz_drop_document_writer(param.ctx, param.out);
		fz_drop_buffer(param.ctx, buf);
	}
	fz_catch(param.ctx)
	{
		// fz_report_error(ctx);
		retval = 1;
	}

	fz_drop_context(param.ctx);
    return retval;
}
