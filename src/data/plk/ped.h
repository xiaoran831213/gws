#ifndef _GWAS_PLINK_PED_H_
#define _GWAS_PLINK_PED_H_

#include <string>

#define PED "ped"
namespace dat
{
	namespace plk
	{
		void parse_ped(std::string path);	// parse phenotype from *.ped file.
	}
}
#endif