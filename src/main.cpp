#include <stdio.h>
#include "opts.h"
#include <dbms/dbms.h>
#include <data/data.h>
#include <qcsv.h>
#include <string>
#include <typedefs.h>

// make sure stdin and stdout stream data exactly as it was,
// without changing unix line feed '\n' to windows "\r\n"
#ifdef	_MSC_VER
#include <io.h>
#include <fcntl.h>
#define SET_BINARY_STDI() _setmode(_fileno(stdin),	_O_BINARY)
#define SET_BINARY_STDO() _setmode(_fileno(stdout),	_O_BINARY)
#else
#define SET_BINARY_STDI()
#define SET_BINARY_STDO()
#endif

using namespace std;
int main(int argc, char* argv[])
{
	SET_BINARY_STDI();
	SET_BINARY_STDO();
	try
	{
		db::open();
		CmdPsr optpsr("GWSX Root Options", ' ', VERSION);
		CmdArg<string>::Ulb arg_op_type("optype", "optype", true, "data", "string", optpsr);
		optpsr.parse(2, argv);
		if (arg_op_type.getValue() == "data")
		{
			dat::open(argc - 1, argv + 1);
		}
		else
		{
			printf("Stub branch\n");
		}
		db::shut();
	}
	catch (exp_base e)
	{
		std::cerr << boost::diagnostic_information(e);
	}
	catch (std::exception e)
	{
		std::cerr << boost::diagnostic_information(e);
	}
	getchar();
	return 0;
}