#ifndef _GWAS_CSVS_H_
#define _GWAS_CSVS_H_

#include <opts.h>
#include <string>

namespace dat
{
	namespace csv
	{
		bool open(int argc, char* argv[]);
		bool shut();
	}
}

#endif