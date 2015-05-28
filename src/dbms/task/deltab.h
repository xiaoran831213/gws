#ifndef _GWAS_DBMS_DROP_TABLE_TASK_H_
#define _GWAS_DBMS_DROP_TABLE_TASK_H_

#include "task.h"
namespace db
{
	namespace tsk
	{
		class DelTab;
	}
}

class db::tsk::DelTab :public virtual db::tsk::Task
{
private:
	std::string m_tab;	// table name
public:
	DelTab() :DelTab(std::string())
	{
	}
	DelTab(const std::string& tab) :m_tab(tab)
	{
	}
	virtual ~DelTab() {}

	void tab(const std::string& tab)	// the target table
	{
		m_tab = tab;
	}
	const std::string& tab() const	// the target table
	{
		return m_tab;
	}
public:
	std::string sql() const override;
};
#endif