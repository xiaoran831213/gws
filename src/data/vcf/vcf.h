#ifndef _VCF_PARSER_
#define _VCF_PARSER_
#include <data/data.h>

// ped or fam file's first six columns
#define VCS	"vcs"	// Variant Call Sites
#define VID "vid"	// variant id
#define CHR "chr"	// chromosome
#define BSP "bsp"	// physics position
#define AL1 "al1"	// allele 1, minor allele
#define AL2 "al2"	// allele 2, major allele
#define IFO "ifo"	// INFO field
#define QLY "qly"	// QUALITY field
namespace dat
{
	namespace vcf
	{
		int open(int argc, char* argv[]);
		int shut();
		class exp_vcf :public exp_dat {};
	}
}

#endif