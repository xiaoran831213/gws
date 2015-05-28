#include "newtab.h"
#include <sstream>
#include <iostream>
using namespace std;
string db::tsk::NewTab::sql() const
{
	if (m_tab.empty())
		GEXP(db::tsk::exp_tsk() << db::tsk::type("NewTab") << errs("empty table name"));

	stringstream out;
	// DROP TABLE IF EXISTS ...
	if (m_ovr)
		out << "DROP TABLE IF EXISTS " << m_tab << ";" << std::endl;

	// CREATE TABLE ...
	out << "CREATE TABLE IF NOT EXISTS " << m_tab;
	out << "\n(\n";
	for (SVec::Ctr b = m_col.p0(), e = m_col.pN(), c = b; c < e; c++)	// column definitions
	{
		if (c > b)
			out << ",\n";
		out << "\t" << *c;
	}

	if (m_key.size())
		out << ",\n\tPRIMARY KEY (";
	for (SVec::Ctr b = m_key.p0(), e = m_key.pN(), k = b; k < e; k++)	// primary key
	{
		if (b < k)
			out << ", ";
		out << *k;
	}
	if (m_key.size())
		out << ")";

	out << "\n)";
	if (!m_rid)
		out << " WITHOUT ROWID";
	out << "\n";
	return out.str();
}