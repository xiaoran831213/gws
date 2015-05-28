#if (defined(_WIN32) || defined(WIN32)||defined(_MSC_VER))
#pragma warning(disable:4996)
#endif

#include "task.h"
#include <exp.h>
#include "../dbms.h"
#include <string>
using namespace db::tsk;

void Task::exec()
{
	if (m_stmt)
		sqlite3_finalize(m_stmt);

	char cstr_sqls[4096];
	strncpy(cstr_sqls, sql().c_str(), 4096);
	const char *p = cstr_sqls;

	int rc;
	while (p[0])
	{
		rc = sqlite3_prepare_v2(db::me(), p, -1, &m_stmt, &p);	// compile sqlite statement
		if (SQLITE_OK != rc)
			GEXP(exp_prp() << sqls(p) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));

		rc = sqlite3_step(m_stmt);
		if (SQLITE_DONE != rc && SQLITE_ROW != rc)
			GEXP(exp_run() << sqls(p) << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(db::me())) << clzz(__CLZZ__));
	}
}