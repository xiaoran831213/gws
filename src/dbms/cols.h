/*Database Column Definition.*/
#ifndef _DBMS_COLUMN_SET_
#define _DBMS_COLUMN_SET_
#include "dbms.h"
#include <string>
namespace db
{
	/* ---------- Exceptions ---------- */
	struct exp_col :virtual exp_db{};
	typedef boost::error_info<struct _column_name, std::string>	col_name;
	typedef boost::error_info<struct _column_type, std::string>	col_type;
	struct Column;
}

/* Column definition.*/
struct db::Column
{
	std::string	name;	// column name
	int			type;	// column type
	int			pkey;	// primary key

	/* New column definition.
	name --- column name.
	type --- column type, should be one of flowing const c string:
	INT -- INTEGER,
	FLT -- FLOAT/REAL,
	BLB -- BLOG,
	TXT -- TEXT (default).
	pkey --- positive and negative denotes ascending, descending primary key respectively,
	while 0 means a normal column.*/
	Column(std::string name, int type = 0, int pkey = 0) :
		name(name),
		type(type),
		pkey(pkey)
	{}
};
#endif