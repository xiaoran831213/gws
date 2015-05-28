#ifndef _DBMS_INSERT_TASK_
#define _DBMS_INSERT_TASK_

#include "task.h"
#include <qvec.h>
#include "occ.h"
#include "sqlite3.h"
#include <exp.h>
#include "argmnp.h"
#include <sstream>
#include <iostream>
namespace db
{
	namespace tsk
	{
		class InsRow;
	}
}

class db::tsk::InsRow :public Task, public ArgMnp, public Occ
{
private:
	SVec			m_col;		// column name vector.
	std::string			m_tab;		// table name
	bool			m_ovr;		// overwrite existing table
public:
	InsRow() :InsRow(std::string())
	{
	}
	InsRow(const std::string& tab) :m_tab(tab), Occ()
	{
	}
	virtual ~InsRow() {}

	/* set column tags.*/
	template<typename... NS>
	SVec& col(NS... names)
	{
		if (m_stmt)
		{
			sqlite3_finalize(m_stmt);
			m_stmt = 0;
		}
		return m_col(names...);
	}
	SVec& col()
	{
		return m_col;
	}

	/* set table name.*/
	void tab(std::string tid)		// the target table
	{
		if (m_stmt)
		{
			sqlite3_finalize(m_stmt);
			m_stmt = 0;
		}
		m_tab = tid;
	}
	const std::string& tab() const	// the target table
	{
		return m_tab;
	}

	/* Implement void Task::sql(FILE*)
	Generate INSERT (...) VALUE(...) script, and send to output.*/
	std::string sql() const override;

public:
	/* After execution, the insertion task resets itself for the next round.*/
	void exec() override;
	void next();
};
#endif
