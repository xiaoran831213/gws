#include "query.h"
#include <vector>
#include <dbms/dbms.h>

using namespace std;
using namespace db::tsk;

void Query::exec()
{
	if (m_stmt)
		sqlite3_finalize(m_stmt);

	int rc = sqlite3_prepare_v2(db::me(), sql().c_str(), -1, &m_stmt, 0);	// compile sqlite statement
	if (SQLITE_OK != rc)
		GEXP(exp_prp() << sqls(sql()) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));
}

bool Query::more()
{
	int rc = sqlite3_step(m_stmt);
	if (SQLITE_ROW == rc)	// see another row
		return true;
	else if (SQLITE_DONE == rc)	// no more
		return false;
	else
		GEXP(exp_prp() << sqls(sql()) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));
}

int Query::print(std::ostream& out, bool head, std::set<std::string>& cset) const
{
	int rc = SQLITE_OK;
	sqlite3_stmt* s;
	rc = sqlite3_prepare_v2(db::me(), sql().c_str(), -1, &s, 0);	// compile sqlite statement
	if (SQLITE_OK != rc)
		GEXP(exp_prp() << sqls(sql()) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));

	vector<int> indices;
	for (int i = 0, n = sqlite3_column_count(s); i < n; i++)
	{
		if (cset.empty() || cset.count(sqlite3_column_name(s, i)))
		{
			indices.push_back(i);
			if (head)
			{
				if (i > 0)
					out << '\t';
				out << sqlite3_column_name(s, i);
			}
		}
	}
	out << '\n';

	int npr = 0;	// number of printed row
	while (SQLITE_ROW == (rc = sqlite3_step(s)))
	{
		for (vector<int>::iterator b = indices.begin(), e = indices.end(), p = b; p < e; p++)
		{
			if (p > b)
				out << '\t';
			switch (sqlite3_column_type(s, *p))
			{
			case SQLITE_BLOB:
				out.write((cstr)sqlite3_column_blob(s, *p), sqlite3_column_bytes(s, *p));
				break;
			case SQLITE_NULL:
				out.write("NULL", 4);
				break;
			case SQLITE_TEXT:
				out.write((cstr)sqlite3_column_text(s, *p), sqlite3_column_bytes(s, *p));
				break;
			case SQLITE_INTEGER:
				out.write((cstr)sqlite3_column_text(s, *p), sqlite3_column_bytes(s, *p));
				break;
			case SQLITE_FLOAT:
				out.write((cstr)sqlite3_column_text(s, *p), sqlite3_column_bytes(s, *p));
				break;
			}
		}
		out << '\n';
		npr++;
	}
	if (SQLITE_DONE == rc)	// no more
		return npr;
	else
	{
		exp_qry e;
		e << sqls(sqlite3_sql(s)) << errn(rc)
			<< errs(sqlite3_errstr(rc))
			<< errm(sqlite3_errmsg(db::me()))
			<< clzz(__CLZZ__);
		sqlite3_finalize(s);
		s = 0;
		GEXP(e);
	}
}
