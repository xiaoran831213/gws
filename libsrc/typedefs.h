#ifndef _GWAS_TYPEDEFS_H_
#define _GWAS_TYPEDEFS_H_

typedef unsigned char			byte;
typedef unsigned int			uint;
typedef const char *			cstr;
typedef char *					astr;
typedef void *					amem;	// arbitrary memory
typedef const void *			cmem;	// constant memory
typedef void *					aptr;	// arbitrary pointer
typedef const void *			cptr;	// constant pointer

#if defined(_MSC_VER)
typedef __int64					si64;
typedef unsigned __int64		ui64;
#else
typedef long long int			si64;
typedef unsigned long long int	ui64;
#endif

typedef size_t	sztp;

#ifndef KB
#define KB 0x00000400
#endif

#ifndef MB
#define MB 0x00010000
#endif

#ifndef sstr
#define sstr std::string
#endif

#endif