#include "qcsv.h"
#include "zlib/zlib.h"
#include <exp.h>

#define SPAN 1048576L	/* desired distance between access points */
#define WNDS 512*KB	/* inflation window size */
#define CHNK 128*KB	/* zip file input chunk size */

void QCsv::on_word()	// on regular word feed
{
#if(defined(QCSV_DEBUG) && QCSV_DEBUG>0)
	if (icol() > 0)
		fputc(sptr(), stdout);
	fprintf(stdout, "%s", wptr());
#endif
}
void QCsv::on_line()	// on regular line feed
{
#if(defined(QCSV_DEBUG) && QCSV_DEBUG>0)
	fputc('\n', stdout);
#endif
}
void QCsv::on_init()	// before everything
{}
void QCsv::on_head()	// finish of header
{}
void QCsv::on_fini()	// finish of file
{}
void QCsv::on_ckey()	// on col key (words in the head)
{}
void QCsv::on_rkey()	// on row key (1st word of a row)
{
}
void QCsv::proc()
{
	if (!m_file)
		return;

	// parse csv files, fill raw table.
	unsigned char options = CSV_APPEND_NULL;
	csv_parser parser;
	if (csv_init(&parser, options) != 0)
		GEXP(exp_base() << errs("Failed to initialize csv parser."));

	csv_set_delim(&parser, m_sptr);
	m_icol = 0;
	m_irow = 0;
	m_wptr = 0;

	// parse file
	char buf[CHNK];
	sztp bytes_read;
	rewind(m_file);
	on_init();
	while ((bytes_read = fread(buf, 1, CHNK, m_file)) > 0)
	{
		if (csv_parse(&parser, buf, bytes_read, qcsv_proc_word_callback, qcsv_proc_line_callback, this) != bytes_read)
		{
			GEXP(exp_base()
				<< clib("libcsv") << cfun("csv_parse")
				<< errn(csv_error(&parser))
				<< errm(csv_strerror(csv_error(&parser))));
		}
	}
	csv_fini(&parser, qcsv_proc_word_callback, qcsv_proc_line_callback, this);
	m_nrow = m_irow;
	on_fini();
}
void QCsv::scan()
{
	if (!m_file)
		return;

	// parse csv files, fill raw table.
	unsigned char options = CSV_APPEND_NULL;
	csv_parser parser;
	if (csv_init(&parser, options) != 0)
		GEXP(exp_base() << errs("Failed to initialize csv parser."));

	csv_set_delim(&parser, m_sptr);

	// parse file
	rewind(m_file);
	char buf[CHNK];
	sztp bytes_read;

	while ((bytes_read = fread(buf, 1, CHNK, m_file)) > 0)
	{
		if (csv_parse(&parser, buf, bytes_read, qcsv_scan_word_callback, qcsv_scan_line_callback, this) != bytes_read)
		{
			GEXP(exp_base()
				<< clib("libcsv") << cfun("csv_parse")
				<< errn(csv_error(&parser))
				<< errm(csv_strerror(csv_error(&parser))));
		}
	}
	csv_fini(&parser, qcsv_scan_word_callback, qcsv_scan_line_callback, this);
	m_nrow = m_irow;
}

void ZCsv::proc()
{
	if (!m_file)
		return;

	// parse csv files, fill raw table.
	unsigned char options = CSV_APPEND_NULL;
	csv_parser parser;
	if (csv_init(&parser, options) != 0)
		GEXP(exp_base() << errs("Failed to initialize csv parser."));

	csv_set_delim(&parser, m_sptr);
	m_icol = 0;
	m_irow = 0;
	m_wptr = 0;

	// parse file
	si64 fzip_size = 0;
	si64 fzip_pass = 0;
	fseek(m_file, 0L, SEEK_END);
	fzip_size = ftell(m_file);
	fseek(m_file, 0L, SEEK_SET);
	int ret;
	byte buf[CHNK];
	byte wnd[WNDS];

	z_stream zs;	/*zlib context*/
	zs.zalloc = Z_NULL;
	zs.zfree = Z_NULL;
	zs.opaque = Z_NULL;
	zs.avail_in = 0;
	zs.next_in = Z_NULL;
	ret = inflateInit2(&zs, 47);      /* automatic zlib or gzip decoding */
	if (ret != Z_OK)
		GEXP(exp_file() << clib("zlib") << cfun("inflateInit2") << errn(ret) << errm(zs.msg));
	JEXP_INIT(erro_unzp, exp_file);

	on_init();
	/* inflate the input, maintain a sliding window, also validates the integrity of the
	compressed data using the check information at the end of the gzip or zlib stream */
	time_t t0 = time(0);
	time_t tN = t0;
	double tD = 0.0;
UNZP_LOOP:
	if (zs.avail_in < 2 && feof(m_file))
	{
		/* input is depleted, check the end of zip stream. if zctx.avail_in == 1, the dangling
		last byte is ignorable trailing garbage if Z_STREAM_END has been reached.*/
		if (ret != Z_STREAM_END)
			JEXP(UNZP_STOP, erro_unzp, clib("zlib"), cfun("inflate"), errn(Z_ERRNO), errm("End of input before end of gzip stream."));
		goto UNZP_STOP;
	}
	if (zs.avail_in < 2 && !feof(m_file))
	{
		/* get compressed data from input, be sure at least 2 bytes is ready for gzip header check.*/
		if (zs.avail_in == 1)
			buf[0] = buf[CHNK - 1];
		zs.avail_in += (uint)fread(buf + zs.avail_in, 1, CHNK - zs.avail_in, m_file);
		if (ferror(m_file))
			JEXP(UNZP_STOP, erro_unzp, clib("stdio"), cfun("fread"), errn(errno), errm(strerror(errno)));
		zs.next_in = buf;
		fzip_pass = ftell(m_file);
		time(&tN);
		tD = difftime(tN, t0);
		if (m_fsnk != stdout)
			fprintf(stdout, "%10lld %10lld %5.2f%% time_elps: %8.2fs\r", fzip_pass, fzip_size, 100.0 * fzip_pass / fzip_size, tD);
		else
			fprintf(stderr, "%10lld %10lld %5.2f%% time_elps: %8.2fs\r", fzip_pass, fzip_size, 100.0 * fzip_pass / fzip_size, tD);
		goto UNZP_LOOP;
	}
	if (zs.avail_in > 1)
	{
		/* Compressed data is avaiable. reset the stream context if gzip header is detected,
		otherwise the rest of the stream is trailing garbage which is to be ignored. */
		if (ret == Z_STREAM_END)
		{
			if (zs.next_in[0] != 31 || zs.next_in[1] != 139)
				goto UNZP_STOP;	/*ignore trailing garbage.*/
			ret = inflateReset(&zs);
			if (ret != Z_OK)
				JEXP(UNZP_STOP, erro_unzp, clib("zlib"), cfun("inflateReset"), errn(ret), errm(zs.msg));
			goto UNZP_LOOP;
		}

		/* consume compressed data by inflating it. */
		zs.avail_out = WNDS;					/* reset sliding window*/
		zs.next_out = wnd;
		ret = inflate(&zs, Z_NO_FLUSH);
		if (ret != Z_OK && ret != Z_STREAM_END)
			JEXP(UNZP_STOP, erro_unzp, clib("zlib"), cfun("inflate"), errn(ret), errm(zs.msg));

		/* forward unziped output to csv parser*/
		//fwrite(wnd, 1, WNDS - zs.avail_out, fsnk);
		uint nchr = WNDS - zs.avail_out;
		if (csv_parse(&parser, wnd, nchr, qcsv_proc_word_callback, qcsv_proc_line_callback, this) != nchr)
			JEXP(UNZP_STOP, erro_unzp, clib("libcsv"), cfun("csv_parse"), errn(csv_error(&parser)), errm(csv_strerror(csv_error(&parser))));
		goto UNZP_LOOP;
	}
UNZP_STOP:
	(void)inflateEnd(&zs);
	JEXP_CHCK(erro_unzp);

	csv_fini(&parser, qcsv_proc_word_callback, qcsv_proc_line_callback, this);
	m_nrow = m_irow;
	on_fini();
}

//#define ZCSV_DEBUG 300
void ZCsv::on_word()	// on regular word feed
{
	if (m_fsnk)
	{
		if (icol() > 0)
			fputc(m_sptr, m_fsnk);
		fprintf(m_fsnk, "%s", wptr());
	}
}
void ZCsv::on_line()	// on regular line feed
{
	if (m_fsnk)
	{
		fputc('\n', m_fsnk);
	}
}