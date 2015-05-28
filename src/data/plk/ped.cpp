#include <cstdio>
#include <string>
#include <exp.h>
#include <libcsv.h>
#include <dbms/dbms.h>
#include <dbms/task/deltab.h>
#include <dbms/task/newtab.h>
#include <dbms/task/insert.h>
#include <opts.h>
#include <data/data.h>
#include "plk.h"
#include "ped.h"
using namespace std;
struct parse_context
{
	size_t icol = 0;
	size_t irow = 0;
	db::tsk::InsRow task;
	FILE* tmpf = 0;
};

// call back function on new word
void cb_word(void *s, size_t len, void *data)
{
	if (!len)
		return;
	parse_context *ctx = (parse_context *)data;
	char* word = (char*)s;
	switch (ctx->icol)
	{
	case 0:			//fid
		ctx->task.bind(0, word);
		break;
	case 1:			//iid
		ctx->task.bind(1, word);
		break;
	case 2:			//pid
		ctx->task.bind(2, word);
		break;
	case 3:			//mid
		ctx->task.bind(3, word);
	case 4:			//sex
		ctx->task.bind(4, word);
		break;
	case 5:			//phe
		ctx->task.bind(5, word);
		break;
	case 6:			//gno start
		ctx->tmpf = tmpfile();
		fwrite(s, len, 1, ctx->tmpf);
		break;
	default:		//gno rest
		fwrite(s, len, 1, ctx->tmpf);
		// skip the BLOB transfer for now.
		break;
	}
	ctx->icol++;
}

// call back on line finish
void cb_line(int c, void *data)
{
	parse_context *ctx = (parse_context *)data;
	//	ctx->task.bind(6, ctx->tmpf);	// bind GNO BLOG
	fclose(ctx->tmpf);

	ctx->task.exec();	// execute INSERT sql
	ctx->icol = 0;
	ctx->irow++;
}

static int is_space(unsigned char c)
{
	if (c == CSV_SPACE || c == CSV_TAB)
		return 1;
	return 0;
}
static int is_term(unsigned char c)
{
	if (c == CSV_CR || c == CSV_LF) return 1;
	return 0;
}

void dat::plk::parse_ped(std::string pedPath)
{
	db::tsk::NewTab newTab;
	newTab.tab(MDB"."PED);
	newTab.col<string>(FID);
	newTab.col<string>(IDV);
	newTab.col<string>(PID);
	newTab.col<string>(MID);
	newTab.col<int>(SEX);
	newTab.col<int>(PHE);
	newTab.col<amem>(GNO);
	newTab.ovr(false);
	newTab.key(FID, IDV);

	std::cout << newTab.sql();
	newTab.exec();

	FILE *file;
	struct csv_parser parser;
	struct parse_context context;

	// initialize parser
	unsigned char options = CSV_APPEND_NULL;
	if (csv_init(&parser, options) != 0)
		BOOST_THROW_EXCEPTION(exp_base() << errs("Failed to initialize csv parser."));
	csv_set_space_func(&parser, is_space);
	csv_set_term_func(&parser, is_term);
	csv_set_delim(&parser, CSV_SPACE);

	// open source file
	file = fopen(pedPath.c_str(), "rb");
	if (!file)
	{
		GEXP(exp_file()
			<< path(pedPath)
			<< mode("rb")
			<< clib("stdio")
			<< cfun("fopen")
			<< errn(errno)
			<< errs(strerror(errno)));
	}

	// parse file
	char buf[256];
	size_t bytes_read;

	context.task.col("fid", "iid", "pid", "mid", "sex", "phe", "gno");
	context.task.tab(MDB"."PED);
	context.task.occ(db::tsk::Occ::RPL);
	while ((bytes_read = fread(buf, 1, 256, file)) > 0)
	{
		if (csv_parse(&parser, buf, bytes_read, cb_word, cb_line, &context) != bytes_read)
		{
			fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(&parser)));
		}
	}
	csv_fini(&parser, cb_word, cb_line, &context);

	if (ferror(file))
		fprintf(stderr, "Error while reading file %s\n", pedPath);
	fclose(file);
	return;
}