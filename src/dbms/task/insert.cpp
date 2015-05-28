#include "insert.h"
#include <dbms/dbms.h>
using namespace db::tsk;
using namespace std;
string InsRow::sql() const
{
	// check table name
	if (m_tab.empty())
		GEXP(db::tsk::exp_tsk() << errs("Empty table name."));
	if (!m_col.n())
		GEXP(db::tsk::exp_tsk() << errs("Empty column list."));
	std::stringstream out;
	// Check On Conflict Clause. For INSERT script, use IGNORE as default.
	out << "INSERT OR " << m_occ << " INTO " << m_tab << "\n\t(";
	for (SVec::Ctr b = m_col.p0(), e = m_col.pN(), c = b; c < e; c++)	// column identifiers.
	{
		if (c > b)
			out << ", ";
		out << *c;
	}
	out << ")\nVALUES\n\t(";
	for (SVec::Ctr b = m_col.p0(), e = m_col.pN(), c = b; c < e; c++)	// value parameters.
	{
		if (c > b)
			out << ", ";
		out << "@" << *c;
	}
	out << ")\n";
	return out.str();
}

void InsRow::exec()
{
	sqlite3_finalize(m_stmt);	// Invoking finalize() on a NULL pointer is a harmless no - op
	int rc = sqlite3_prepare_v2(db::me(), sql().c_str(), -1, &m_stmt, 0);		// compile sqlite statement
	if (SQLITE_OK != rc)
		GEXP(exp_prp() << sqls(sql()) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));
}

void InsRow::next()
{
	if (!m_stmt)
		GEXP(exp_run() << sqls(sql()) << errm("INSERT statement is not in execution state.") << clzz(__CLZZ__));

	int rc = SQLITE_OK;
	rc = sqlite3_step(m_stmt);
	if (SQLITE_DONE != rc)
		GEXP(exp_run() << sqls(sql()) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));

	rc = sqlite3_reset(m_stmt);
	if (SQLITE_OK != rc)
		GEXP(exp_fin() << sqls(sql()) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));
}