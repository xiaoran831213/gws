#ifndef _GWAS_DATA_H
#define _GWAS_DATA_H

#include <exp.h>
#define SBJ "sbj"		// subject table name
#define PRP "prp"		// property column name, property table name
#define VAL "val"		// value column name
#define RAW "raw"		// raw name, raw id, ect.
#define MIX "mix"		// mix of all data
#define TID "sid"		// text id
#define RID "_rowid_"	// sqlite row number id

namespace dat
{
	bool open(int argc, char* argv[]);
	bool shut();
	class exp_dat :public exp_base {};
}
#endif