#ifndef _GWAS_DBMS_TASK_ARGMNP_H_
#define _GWAS_DBMS_TASK_ARGMNP_H_
#include "task.h"
#include <typedefs.h>
namespace db
{
	namespace tsk
	{
		/* ---------- Exceptions ---------- */
		typedef boost::error_info<struct _arg_name, std::string>  argn;	// argument name
		typedef boost::error_info<struct _arg_type, std::string>  argt;	// argument type
		typedef boost::error_info<struct _arg_count, size_t> argc;	// argument count
		typedef boost::error_info<struct _arg_index, size_t> argi;	// argument index
		struct exp_arg : virtual exp_tsk {};	// Statement argument error

		class ArgMnp : virtual public TaskContext
		{
		public:
			/* NULL argument.*/
			typedef struct{} null;

			// specialized char array binder.
			template<unsigned int N>
			void bind(int i, const char v[N])
			{
				bind(i, v, N);
			}

			template<typename U>
			U get(int i)
			{
				U o;
				return get(i, o);
			}

			void bind(int i, null v);
			void bind(int i, cstr v, int n = -1);
			void bind(int i, cmem v, int n);
			void bind(int i, std::string v);
			void bind(int i, bool v);
			void bind(int i, char v);
			void bind(int i, short v);
			void bind(int i, unsigned short v);
			void bind(int i, int v);
			void bind(int i, unsigned int v);
			void bind(int i, long v);
			void bind(int i, unsigned long v);
			void bind(int i, sqlite3_int64 v);
			void bind(int i, sqlite3_uint64 v);
			void bind(int i, float v);
			void bind(int i, double v);

			char get(int i, char& r);
			bool get(int i, bool& r);
			short get(int i, short& r);
			unsigned short get(int i, unsigned short& r);
			int get(int i, int& r);
			unsigned int get(int i, unsigned int& r);
			long get(int i, long& r);
			unsigned long get(int i, unsigned long& r);
			sqlite3_int64 get(int i, sqlite3_int64& r);
			sqlite3_uint64 get(int i, sqlite3_uint64& r);
			float get(int i, float& r);
			double get(int i, double& r);
			cstr get(int i, cstr& r);
			cmem get(int i, cmem& r);
			std::string get(int i, std::string& r);
			astr cpy(int i, astr* p);
			amem cpy(int i, amem* p);
			astr prt(int i, astr& r);
			amem prt(int i, amem& r);
		};
	}
}
#endif