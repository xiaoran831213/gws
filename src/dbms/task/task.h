#ifndef _DBMS_TASK_H_
#define _DBMS_TASK_H_
#include <cstdio>
#include <sqlite3.h>
#include <dbms/dbms.h>
#include <exp.h>
namespace db
{
	/*Database Task. The purpose of Task is to produce proper SQL script based on supplied context.
	Task and its descends provide efficient interfaces to fetch necessary context to generate SQL.*/
	namespace tsk
	{
		/* ---------- Exceptions ---------- */
		struct exp_tsk : virtual exp_db {};
		typedef boost::error_info<struct _task_sqls, std::string> sqls;	// SQL script.
		typedef boost::error_info<struct _task_type, std::string> type;	// task type

		struct exp_prp : virtual exp_tsk {};	// SQL compile error
		struct exp_run : virtual exp_tsk {};	// SQL runtime error.
		struct exp_fin : virtual exp_tsk {};	// SQL finalization error.
		class TaskContext
		{
		protected:
			TaskContext() :m_stmt(0)
			{}
			sqlite3_stmt*	m_stmt = 0;
			virtual ~TaskContext()
			{
				if (m_stmt)
				{
					sqlite3_finalize(m_stmt);	// free sqlite statement
				}
			}
		};
		/*The basic interface of Task classes.*/
		class Task : virtual public TaskContext
		{
		public:
			Task()	{}
			/*Generate SQL script and write to output file stream.*/
			virtual std::string sql() const = 0;
			virtual void exec();
		};
		class QTsk : virtual public Task
		{
		private:
			std::string m_qsql;
		public:
			QTsk(std::string sql = std::string(), bool suspense = false) :m_qsql(sql)
			{
				if (!suspense)
					exec();
			}
			std::string sql(std::string sql)
			{
				return m_qsql = sql;
			}
			std::string sql() const override
			{
				return m_qsql;
			}
		};
	}
}
#endif
