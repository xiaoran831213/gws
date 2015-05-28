#include <string>
#include <dbms/task/task.h>
#include <opts.h>
#include <qcsv.h>
#include <fstream>
#include <typedefs.h>
#include "plk.h"
#include "bed.h"
#include "ped.h"

using namespace db::tsk;
using namespace std;
bool dat::plk::open(int argc, char* argv[])
{
	CmdPsr plk_psr("plink file minipulator", ' ', VERSION);
	CmdArg<string>::Val arg_ped("", "ped", "input plink PED file(s)", false, "", "string", plk_psr);
	CmdArg<string>::Val	arg_bed("", "bed", "input plink BED file(s)", false, "", "string", plk_psr);
	CmdArg<bool>::Swt	arg_hdb("", "hdb", "rebuild HDB to reflect new data.", plk_psr, false);

	CmdArg<string>::Val	arg_rng("", "rng", "range list.", false, "", "string", plk_psr);
	CmdArg<uint>::Val	arg_bdr("", "bdr", "range border size.", false, 0, "integer", plk_psr);
	CmdArg<uint>::Val	arg_chr("", "chr", "range - chromosome.", false, 0, "integer", plk_psr);
	CmdArg<uint>::Val	arg_bp1("", "bp1", "range - base pair 1.", false, 0, "integer", plk_psr);
	CmdArg<uint>::Val	arg_bp2("", "bp2", "range - base pair 2.", false, 0, "integer", plk_psr);

	CmdArg<string>::Val	arg_out("", "out", "output name.", false, "", "string", plk_psr);
	CmdArg<bool>::Swt	arg_out_ped("", "to-ped", "output to plink PED.", plk_psr, false);
	CmdArg<bool>::Swt	arg_out_bed("", "to-bed", "output to plink BED.", plk_psr, false);
	CmdArg<bool>::Swt	arg_out_txt("", "to-txt", "output to text file.", plk_psr, false);
	CmdArg<bool>::Swt	arg_out_inf("", "to-inf", "output range info.", plk_psr, false);

	plk_psr.parse(argc, argv);

	Bed gno;
	if (arg_hdb.getValue())	// drop mirrored HD tables, parsing procedures will rebuild them.
	{
		std::cout << "Cleanup existing HDB tables.\n";
		QTsk("DROP TABLE IF EXISTS "HDB"."FAM);
		QTsk("DROP TABLE IF EXISTS "HDB"."BIM);
		QTsk("DROP TABLE IF EXISTS "HDB"."BED);
	}
	if (arg_ped.isSet())
	{
		//parse_ped(arg_ped.getValue());
		return false;
	}
	else if (arg_bed.isSet())
	{
		gno = parse_bed(arg_bed.getValue());
	}
	else
	{
		GEXP(exp_plk() << errm("At least one input format should be specified (PED or BED)."));
	}

	// collect genome ranges.
	stringstream ssRng;
	ifstream fsRng;
	ofstream fsOut;
	std::ostream *pOut = 0;
	std::istream *pRng = 0;

	if (arg_chr.isSet())				// one line range list from command line
	{
		int chr = arg_chr.getValue();
		int bp1 = arg_bp1.getValue() - arg_bdr.getValue();
		int bp2 = arg_bp2.getValue() + arg_bdr.getValue();
		ssRng << arg_out.getValue() << "\t" << chr << "\t" << bp1 << "\t" << bp2 << "\n";
		pRng = &ssRng;
	}
	if (arg_rng.isSet())
	{
		fsRng.open(arg_rng.getValue());	// range list from file(no header line!!!)
		pRng = &fsRng;
	}
	if (arg_out.isSet())			// output stream
	{
		if (arg_out.getValue().empty())
			pOut = &std::cout;
		else
		{
			fsOut.open(arg_out.getValue());
			pOut = &fsOut;
		}
	}
	assert(pOut);

	if (pRng)
	{
		if (arg_out_txt.isSet())	// extract ranges.
		{
			pRng->seekg(0, ios_base::beg);
			gno.toRaw(*pRng, arg_out.getValue(), arg_bdr.getValue());
		}
		if (arg_out_inf.isSet())	// report range statistics
		{
			pRng->seekg(0, ios_base::beg);
			gno.ptRng(*pRng, *pOut, arg_bdr.getValue());
		}
	}
	return true;
}
bool dat::plk::shut()
{
	return true;
}

