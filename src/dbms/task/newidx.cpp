#include "newidx.h"
#include <sstream>
#include <iostream>
using namespace db::tsk;
using namespace std;
string NewIdx::sql() const
{
	string db = m_tab.substr(0, m_tab.find("."));
	string tb = m_tab.substr(m_tab.find(".")+1, m_tab.length());
	ostringstream out;
	out << "CREATE";
	if (m_unq)
		out << " UNIQUE";
	out << " INDEX IF NOT EXISTS ";
	out << db << ".ix";
	for (SVec::Ctr b = m_col.p0(), e = m_col.pN(), c = b; c < e; c++)
		out << "_" << *c;
	out << " ON " << tb << " (";
	for (SVec::Ctr b = m_col.p0(), e = m_col.pN(), c = b; c < e; c++)
	{
		if (b < c)
			out << ", ";
		out << *c;
	}
	out << ")";
	return out.str();
}