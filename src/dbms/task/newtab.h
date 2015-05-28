#ifndef _DBMS_NEW_TAB_TASK_
#define _DBMS_NEW_TAB_TASK_

#include "task.h"
#include <qvec.h>
#include "../cols.h"
#include <iostream>

namespace db
{
	namespace tsk
	{
		class NewTab;
	}
}

class db::tsk::NewTab :public virtual db::tsk::Task
{
private:
	std::string	m_tab;		// table name
	bool	m_ovr;		// overwrite existing table
	SVec	m_col;
	SVec	m_key;
	bool	m_rid = true;		// use Sqlite Row id?
	QVec<SVec> m_vidx;
public:
	NewTab() :NewTab(std::string())
	{
	}
	NewTab(const std::string& tab) :m_tab(tab)
	{
	}
	virtual ~NewTab() {}


	const std::string& tab(const std::string& tab)
	{
		return m_tab = tab;
	}
	const std::string& tab() const
	{
		return m_tab;
	}

	bool ovr(bool ovr)	/*Overwrite existing table?*/
	{
		return m_ovr = ovr;
	}
	bool ovr()				/*Overwrite existing table?*/
	{
		return m_ovr;
	}

	bool rid(bool rid)
	{
		return m_rid = rid;
	}
	bool rid()
	{
		return m_rid;
	}

	template<typename T>
	NewTab& col(std::string column_name)
	{
		m_col(column_name + " " + db::Cast<T>::NAME);
		return *this;
	}

	template<typename ...Names>
	NewTab& key(std::string column_name, Names...rest)
	{
		m_key(column_name, rest...);
		return *this;
	}


	/* Implement void Task::sql(FILE*)
	Generate CREATE TABLE script.*/
	std::string sql() const override;
};
#endif