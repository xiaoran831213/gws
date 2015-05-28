#include <string>
#include <dbms/task/task.h>
#include <dbms/dbms.h>
#include <opts.h>
#include <fstream>
#include <misc.h>
#include "vcf.h"
#include <stdlib.h>
#include <stdio.h>
#include <qcsv.h>
#include <qvec.h>
#include <zlib/zlib.h>
#include <boost/exception/all.hpp>
#include <typedefs.h>

using namespace std;
using namespace db::tsk;

#define SPAN 1048576L	/* desired distance between access points */
#define WNDS 512*KB	/* inflation window size */
#define CHNK 128*KB	/* zip file input chunk size */

struct exp_vcf :virtual exp_base {};
class ZVcf : public ZCsv
{
public:
	ZVcf(FILE* fptr) :ZCsv(fptr, QCsv::SEP('\t'))
	{
	}
private:

protected:
	virtual bool is_cmmt(cstr word, size_t size) override
	{
		return size > 1 && word[0] == '#' && word[1] == '#';
	}
	virtual void on_word() override
	{
		if (icol() > 0)
			fputc(sptr(), stdout);
		fprintf(stdout, "%s ", wptr());
	}
	virtual void on_line() override
	{
		fprintf(stdout, "\n");
	}
	virtual void on_ckey() override
	{
		if (icol() > 0)
			fputc(sptr(), stdout);
		fprintf(stdout, "%s ", wptr());
	}
	virtual void on_head() override
	{
		fprintf(stdout, "\n");
	}
};

int dat::vcf::open(int argc, char* argv[])
{
	CmdPsr plk_psr("VCF file minipulator", ' ', VERSION);
	CmdArg<string>::Mlt	arg_whr("", "whr", "Where are the VCFs?", true, "string", plk_psr);
	CmdArg<bool>::Swt	arg_gzp("", "gzp", "input VCF is gziped", plk_psr, false);
	CmdArg<bool>::Swt	arg_hdb("", "hdb", "rebuild HDB to reflect new data.", plk_psr, false);

	CmdArg<string>::Val	arg_rng("", "rng", "range list.", false, "", "string", plk_psr);
	CmdArg<uint>::Val	arg_bdr("", "bdr", "range border size.", false, 0, "integer", plk_psr);
	CmdArg<uint>::Val	arg_chr("", "chr", "range - chromosome.", false, 0, "integer", plk_psr);
	CmdArg<uint>::Val	arg_bp1("", "bp1", "range - base pair 1.", false, 0, "integer", plk_psr);
	CmdArg<uint>::Val	arg_bp2("", "bp2", "range - base pair 2.", false, 0, "integer", plk_psr);

	CmdArg<string>::Val	out_whr("", "out", "output name.", false, "", "string", plk_psr);
	CmdArg<bool>::Swt	out_ped("", "to-ped", "output to plink PED.", plk_psr, false);
	CmdArg<bool>::Swt	out_bed("", "to-bed", "output to plink BED.", plk_psr, false);
	CmdArg<bool>::Swt	out_txt("", "to-txt", "output vcf in text.", plk_psr, false);
	CmdArg<bool>::Swt	out_inf("", "to-vcf", "output vcf in gzip.", plk_psr, false);

	plk_psr.parse(argc, argv);

	if (arg_hdb.getValue())	// drop mirrored HD tables, parsing procedures will rebuild them.
	{
		QTsk("DROP TABLE IF EXISTS "HDB"."VCS);
	}

	FILE* fout = 0;
	if (out_whr.isSet())
	{
		if (out_whr.getValue().length())
		{
			fout = fopen(out_whr.getValue().c_str(), "wb");
			if (!fout)
				GEXP(exp_file() << clib("stdio") << cfun("fopen") << errn(errno) << errs(strerror(errno)) << path(out_whr.getValue()) << mode("wb"));
			setvbuf(fout, 0, _IOFBF, 512 * 1024);
		}
		else
			fout = stdout;
	}

	// VCF.GZIP file
	if (arg_whr.isSet())
	{
		for (CmdArg<string>::Ctr p = arg_whr.begin(), e = arg_whr.end(); p != e; p++)
		{
			JEXP_INIT(erro_fzip, exp_file);
			FILE *fzip = fopen(p->c_str(), "rb");
			if (!fzip)
				GEXP(exp_file() << path(p->c_str()) << mode("wb") << clib("stdio") << cfun("fopen") << errn(errno) << errs(strerror(errno)));
			ZCsv zcsv(fzip, QCsv::SEP('\t'), QCsv::SNK(fout));
			zcsv.proc();
			fclose(fzip);
		}
	}
	if (fout != NULL && fout != stdout)
	{
		fclose(fout);
	}
	return true;
}

