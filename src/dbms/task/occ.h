/** On Conflict Clasuses.*/
#ifndef _GWAS_DBMS_OCCS_H_
#define _GWAS_DBMS_OCCS_H_
#include <string>
namespace db
{
	namespace tsk
	{
		/*The On Conflict Clauses (occ) support.
		Applies to UNIQUE and NOT NULL constraints (and to PRIMARY KEY constraints which for the purposes of this
		section are the same thing as UNIQUE constraints). The ON CONFLICT algorithm does not apply to FOREIGN KEY
		constraints. There are five conflict resolution algorithm choices:
		1.RBK - ROLLBACK
		2.ABT - ABORT
		3.FAL - FAIL
		4.IGN - IGNORE
		5.RPL - REPLACE
		The default conflict policy is IGNORE.*/
		class Occ
		{
		public:
			/*ROLLBACK
			When an applicable constraint violation occurs, the ROLLBACK resolution algorithm aborts the current
			SQL statement with an SQLITE_CONSTRAINT error and rolls back the current transaction. If no
			transaction is active (other than the	implied transaction that is created on every command) then
			the ROLLBACK resolution algorithm works the same as the ABORT algorithm.*/
			static const std::string RBK;
			/*ABORT
			When an applicable constraint violation occurs, the ABORT resolution algorithm aborts the current SQL
			statement with an SQLITE_CONSTRAINT error and backs out any changes made by the current SQL	statement;
			but changes caused by prior SQL statements within the same transaction are preserved and the transaction
			remains active.This is the default behavior and the behavior specified by the SQL standard.*/
			static const std::string ABT;
			/*FAIL
			When an applicable constraint violation occurs, the FAIL resolution algorithm aborts the current SQL
			statement with an SQLITE_CONSTRAINT error.But the FAIL resolution does not back out prior changes of
			the SQL statement that failed nor does it end the transaction.For example, if an UPDATE statement
			encountered a constraint violation on the 100th row that it attempts to update, then the first 99 row
			changes are preserved but changes to rows 100 and beyond never occur.*/
			static const std::string FAL;
			/*IGNORE
			When an applicable constraint violation occurs, the IGNORE resolution algorithm skips the one row that
			contains the constraint violation and continues processing subsequent rows of the SQL statement as if
			nothing went wrong. Other rows before and after the row that contained the constraint violation are
			inserted or updated normally. No error is returned when the IGNORE conflict resolution algorithm is used.*/
			static const std::string IGN;
			/*REPLACE
			When a UNIQUE constraint violation occurs, the REPLACE algorithm deletes pre-existing rows that are
			causing	the constraint violation prior to inserting or updating the current row and the command continues
			executing normally. If a NOT NULL constraint violation occurs, the REPLACE conflict resolution replaces
			the NULL value with the default value for that column, or if the column has no default value, then the
			ABORT algorithm is used. If a CHECK constraint violation occurs, the REPLACE conflict resolution
			algorithm always works like ABORT.*/
			static const std::string RPL;
		protected:
			std::string m_occ = Occ::IGN;
		public:
			Occ(const std::string& occ) :m_occ(occ)
			{}
			Occ() :m_occ(Occ::IGN)
			{}

			/* Set On Conflict Resolution to be used when insertion conflict occur. Must be one of the
			predefined options in db::tsk::occ.
			Available options are:
			RBK --- ROLLBACK
			ABT --- ABORT
			FAL --- FAIL
			IGN --- IGNORE
			RPL --- REPLACE	*/
			const std::string& occ(const std::string& occ)
			{
				return m_occ = occ;
			}

			/* Get On Conflict Resolution to be used when insertion conflict occur. Will be one of the
			predefined options in db::tsk::Occ.
			Available options are:
			RBK --- ROLLBACK
			ABT --- ABORT
			FAL --- FAIL
			IGN --- IGNORE
			RPL --- REPLACE	*/
			const std::string& occ()
			{
				return m_occ;
			}
		};
	}
}
#endif
