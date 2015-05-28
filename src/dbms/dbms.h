#ifndef _GWAS_DBMS_H
#define _GWAS_DBMS_H

#include "sqlite3.h"
#include <typedefs.h>
#include <exp.h>

#define HDB "hddb"		// hard disk db
#define MDB "main"		// in memory db, also the main db

/*GWAS Database*/
namespace db
{
	void open();
	void shut();
	sqlite3* me();
	struct exp_db : virtual exp_base {};

	template<typename T, int C>
	struct SqliteCast
	{
		const static int			CODE = C;
		const static std::string	NAME;
	};

	template<typename T>
	struct Cast :public SqliteCast < T, 0 >
	{
		const static int			CODE = 0;
		const static std::string	NAME;
	};

	template<>
	struct Cast < char >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < unsigned char >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < short >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < unsigned short >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < int >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < unsigned int >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < long >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < unsigned long >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < sqlite3_int64 >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < sqlite3_uint64 >
	{
		const static int			CODE = SQLITE_INTEGER;
		const static std::string	NAME;
	};

	template<>
	struct Cast < float >
	{
		const static int			CODE = SQLITE_FLOAT;
		const static std::string	NAME;
	};

	template<>
	struct Cast < double >
	{
		const static int			CODE = SQLITE_FLOAT;
		const static std::string	NAME;
	};

	template<>
	struct Cast < astr >
	{
		const static int			CODE = SQLITE_TEXT;
		const static std::string	NAME;
	};

	template<>
	struct Cast < cstr >
	{
		const static int			CODE = SQLITE_TEXT;
		const static std::string	NAME;
	};

	template<>
	struct Cast < amem >
	{
		const static int			CODE = SQLITE_BLOB;
		const static std::string	NAME;
	};

	template<>
	struct Cast < cmem >
	{
		const static int			CODE = SQLITE_BLOB;
		const static std::string	NAME;
	};

	template<>
	struct Cast < std::string >
	{
		const static int			CODE = SQLITE_TEXT;
		const static std::string	NAME;
	};

}

#endif