#include "misc.h"
#include <cstdio>
#include <cerrno>
#include <string>
#include <exp.h>
#include "qvec.h"
using namespace std;

#ifdef _MSC_VER
#pragma warning( disable : 4996 )	// disable MSVC deprecation warnning, equivalent to _CRT_SECURE_NO_WARNINGS
#endif

int misc::store_file(char** pmem, FILE *file, size_t* plen)	// helper function
{
	fpos_t pos;
	if (fgetpos(file, &pos))
		GEXP(exp_capi() << clib("stdio") << cfun("fgetpos") << errn(errno) << errs(strerror(errno)));
	size_t len = ftell(file);
	if (len == -1L)
		GEXP(exp_capi() << clib("stdio") << cfun("ftell") << errn(errno) << errs(strerror(errno)));

	try
	{
		*pmem = (char*)malloc(len + 1);
		if (!*pmem)
			GEXP(exp_capi() << clib("stdio") << cfun("malloc") << errn(errno) << errs(strerror(errno)));
		rewind(file);
		size_t get = fread(*pmem, 1, len, file);
		if (plen)
			*plen = get;
		if (get < len)
		{
			fsetpos(file, &pos);	// Try to restore file's initial state before throw exception.
			GEXP(exp_capi() << clib("stdio") << cfun("fread") << errn(errno) << errs(strerror(errno)));
		}
		if (fsetpos(file, &pos))
			GEXP(exp_capi() << clib("stdio") << cfun("fsetpos") << errn(errno) << errs(strerror(errno)));
		((char*)*pmem)[len] = 0;	// terminiate string. the file may not be text, but pending a '\0' do not hurt.
	}
	catch (...)
	{
		if (*pmem)
			free(*pmem);
		*pmem = 0;
		throw;
	}
	return 0;
}
int misc::store_free(char** pmem)
{
	if (*pmem)
		free(*pmem);
	*pmem = 0;
	return 0;
}
int misc::store_file(void** pmem, FILE* file, size_t* plen)
{
	return store_file((char**)pmem, file, plen);
}
int misc::store_free(void** pmem)
{
	return store_free((char**)pmem);
}

static QVec<FILE*> vecf;
FILE* misc::open(std::string p, std::string m)
{
	// open source file
	FILE* file = fopen(p.c_str(), m.c_str());
	if (!file)
	{
		GEXP(exp_file() << path(p) << mode(m)
			<< clib("stdio") << cfun("fopen") << errn(errno) << errs(strerror(errno)));
	}
	return vecf.psh(file);
}
