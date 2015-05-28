#include <dbms/task/newtab.h>
#include <dbms/task/deltab.h>
#include <dbms/cols.h>
#include <opts.h>

#include "data.h"
#include "plk/plk.h"
#include "vcf/vcf.h"
#include "csv/csv.h"
using namespace std;
bool dat::open(int argc, char* argv[])
{
	CmdPsr cmd_psr("Data manipulator options", ' ', VERSION);
	CmdArg<string>::Cst types("plk", "csv", "vcf");
	CmdArg<string>::Ulb opt_type("type", "data type", true, "csv", &types, cmd_psr);
	cmd_psr.parse(2, argv);

	if (opt_type.getValue() == "plk")
	{
		plk::open(argc - 1, argv + 1);
	}
	else if (opt_type.getValue() == "vcf")
	{
		vcf::open(argc - 1, argv + 1);
	}
	else if (opt_type.getValue() == "csv")
	{
		csv::open(argc - 1, argv + 1);
	}
	else
		return false;	// not opened.
	return true;
}
bool dat::shut()
{
	return true;
}
