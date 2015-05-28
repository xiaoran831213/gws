#if defined(_WIN32) || defined(WIN32)||defined(_MSC_VER)
/* This needs to come before any includes for MSVC compiler */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "deltab.h"
#include <exp.h>
using namespace std;

string db::tsk::DelTab::sql() const
{
	// check table name
	if (m_tab.empty())
		GEXP(db::tsk::exp_tsk() << errs("Empty table name."));
	return "DROP TABLE IF EXISTS " + m_tab + "\n";
}
