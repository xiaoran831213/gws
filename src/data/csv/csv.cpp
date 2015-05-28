#include "csv.h"
#include <libcsv.h>
#include <opts.h>
#include <string>
#include <exp.h>
#include <cstdio>
#include <qcsv.h>
using namespace std;

bool dat::csv::open(int argc, char* argv[])
{
	CmdPsr				psr_csvf("CSV file manipulator options", ' ', VERSION);
	CmdArg<string>::Mlt	arg_whr("", "whr", "Where are the CSVs?", true, "string", psr_csvf);
	CmdArg<string>::Val	arg_nxp("", "nxp", "null value regex", false, "", "string", psr_csvf);
	CmdArg<string>::Val	arg_sep("", "spt", "CSV word separator.", false, ",", "char", psr_csvf);
	CmdArg<bool>::Swt	arg_hdr("", "hdr", "CSV has head row.", psr_csvf, false);
	CmdArg<bool>::Swt	arg_hnd("", "hdc", "CSV has hand col.", psr_csvf, false);

	CmdArg<bool>::Swt	arg_gzp("", "gzp", "input CSV is gziped", psr_csvf, false);
	CmdArg<bool>::Swt	arg_hdb("", "hdb", "rebuild HDB to reflect new data.", psr_csvf, false);
	try
	{
		psr_csvf.parse(argc, argv);
	}
	catch (TCLAP::CmdLineParseException e)
	{
		cout << e.argId() << ": " << e.error();
		fgetc(stdin);
	}

	if (arg_whr.isSet())
	{
		for (CmdArg<string>::Ctr p = arg_whr.begin(), e = arg_whr.end(); p != e; p++)
		{
			FILE* fp = fopen(p->c_str(), "rb");
			QCsv csvf(fp, QCsv::SEP(arg_sep.getValue()[0]));
			csvf.proc();
			fclose(fp);
		}
	}
	return true;
}