#ifndef _GWAS_MISC_H_
#define _GWAS_MISC_H_

#include <cstdio>
#include <vector>
#include <string>
#include <typedefs.h>
namespace misc
{
	cstr emsg();
	int store_file(char** pMem, FILE* file, size_t* pLen = 0);
	int store_free(char** pMem);
	int store_file(void** pMem, FILE* file, size_t* pLen = 0);
	int store_free(void** pMem);
	FILE* open(std::string path, std::string mode="rb");
};

#endif
