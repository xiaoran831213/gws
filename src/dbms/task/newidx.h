#ifndef _GWAS_DBMS_NEWIDX__H
#define _GWAS_DBMS_NEWIDX__H

#include "task.h"
#include <qvec.h>

namespace db
{
	namespace tsk
	{
		class NewIdx;
	}
}

class db::tsk::NewIdx :public virtual db::tsk::Task
{
private:
	std::string	m_tab;	// table name
	std::string	m_idx;	// index name
	bool	m_unq;		// a unique index?
	SVec	m_col;		// index columns
public:
	NewIdx() : NewIdx(std::string())
	{
	}
	NewIdx(const std::string& tab) :m_tab(tab)
	{
	}
	virtual ~NewIdx() {}

	const std::string& tab(const std::string& tab)
	{
		return m_tab = tab;
	}
	const std::string& tab() const
	{
		return m_tab;
	}

	bool unq(bool unique)	/*Overwrite existing table?*/
	{
		return m_unq = unique;
	}
	bool unq()				/*Overwrite existing table?*/
	{
		return m_unq;
	}

	template<typename ...Names>
	NewIdx& col(std::string column_name, Names...rest)
	{
		m_col.clr();
		m_col(column_name, rest...);
		return *this;
	}

	/* Implement void Task::sql(FILE*)
	Generate CREATE TABLE script.*/
	std::string sql() const override;
};

#endif