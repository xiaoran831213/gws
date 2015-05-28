#ifndef _DBMS_QUERY_H_
#define _DBMS_QUERY_H_
#include "task.h"
#include "argmnp.h"
#include <string>
#include <ostream>
#include <set>
namespace db
{
	/*Database Task. The purpose of Task is to produce proper SQL script based on supplied context.
	Task and its descends provide efficient interfaces to fetch necessary context to generate SQL.*/
	namespace tsk
	{
		/* ---------- Exceptions ---------- */
		class exp_qry :public exp_tsk{};

		class Query :public Task, public ArgMnp
		{
		public:
			/* Reset the query to 1st row.*/
			void exec() override;
			/* Pop a row of data. Return true if the row is valid, false if the end of query is reached.*/
			bool more();
			int print(std::ostream& ostr, bool head, std::set<std::string>& cset) const;
			int print(std::ostream& ostr, bool head) const
			{
				return print(ostr, head, std::set<std::string>());
			}
			int print(std::ostream& ostr) const
			{
				return print(ostr, true, std::set<std::string>());
			}
			template<typename ... Cols>
			int print(std::ostream& ostr, bool head, std::string next, Cols... rext) const
			{
				std::set<std::string> cset;
				return print(ostr, head, cset, next, rext...);
			}
		private:
			template<typename ... Cols>
			int print(std::ostream& ostr, bool head, std::set<std::string>& cset, std::string next, Cols... rest) const
			{
				cset.insert(next);
				return print(ostr, head, cset, rest...);
			}
		};

		class QQry :public Query
		{
		private:
			std::string m_sql;
		public:
			QQry(std::string sql = std::string(), bool autorun = true) :m_sql(sql)
			{
				Query::exec();
				if (autorun)
					Query::more();
			}
			std::string sql(std::string sql)
			{
				return m_sql = sql;
			}
			std::string sql() const override
			{
				return m_sql;
			}
		};
	}
}
#endif