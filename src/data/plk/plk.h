#ifndef _plk_
#define _plk_

#include <data/data.h>

// ped or fam file's first six columns
#define IDV "iid"	// idividual id
#define FID "fid"	// family id
#define PID "pid"	// parternal id
#define MID "mid"	// maternal id
#define SEX "sex"
#define PHE "phe"

#define EXP "exp"	// gene expression
#define GNO "gno"	// genotype

#define CHR "chr"	// chromosome
#define VID "vid"	// variant id
#define CMP "cmp"	// genetic position
#define BSP "bsp"	// physics position
#define AL1 "al1"	// allele 1, minor allele
#define AL2 "al2"	// allele 2, major allele
namespace dat
{
	namespace plk
	{
		bool open(int argc, char* argv[]);
		bool shut();
		class exp_plk :public exp_dat {};
	}
}

#endif