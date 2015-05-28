#if (defined(_WIN32) || defined(WIN32)||defined(_MSC_VER))
#pragma warning(disable:4996)
#endif

#include <dbms/dbms.h>
#include "argmnp.h"
#include <cstdio>
#include <cstring>

using namespace db::tsk;
using namespace std;

/* Report arguement exception if any happened.*/
template<typename U>
static void bind_check(sqlite3_stmt* s, int i, U v, int e)
{
	if (SQLITE_OK == e)
		return;
	if (s == 0)
	{
		GEXP(exp_tsk()
			<< errn(e)
			<< errs(sqlite3_errstr(e))
			<< errm(sqlite3_errmsg(db::me()))
			<< argi(i)
			<< argt(typeid(v).name()));
	}
	else
	{
		GEXP(exp_arg()
			<< errn(e)
			<< errs(sqlite3_errstr(e))
			<< errm(sqlite3_errmsg(db::me()))
			<< sqls(sqlite3_sql(s))
			<< argi(i)
			<< argc(sqlite3_bind_parameter_count(s))
			<< argt(typeid(v).name()));
	}
}

void ArgMnp::bind(int i, null v)
{
	int e = sqlite3_bind_null(m_stmt, i + 1);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, cstr v, int n)
{
	int e = sqlite3_bind_text(m_stmt, i + 1, v, n, SQLITE_TRANSIENT);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, cmem v, int n)
{
	int e = sqlite3_bind_blob(m_stmt, i + 1, v, n, SQLITE_TRANSIENT);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, string v)
{
	int e = sqlite3_bind_text(m_stmt, i + 1, v.c_str(), -1, SQLITE_TRANSIENT);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, bool v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, char v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, short v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, unsigned short v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, int v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, unsigned int v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, long v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, unsigned long v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, sqlite3_int64 v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, sqlite3_uint64 v)
{
	int e = sqlite3_bind_int64(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, float v)
{
	int e = sqlite3_bind_double(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}
void ArgMnp::bind(int i, double v)
{
	int e = sqlite3_bind_double(m_stmt, i + 1, v);
	bind_check(m_stmt, i, v, e);
}

char ArgMnp::get(int i, char& r)
{
	r = sqlite3_column_int(m_stmt, i);
	return r;
}
bool ArgMnp::get(int i, bool& r)
{
	r = sqlite3_column_int(m_stmt, i) != 0;
	return r;
}
short ArgMnp::get(int i, short& r)
{
	r = sqlite3_column_int(m_stmt, i);
	return r;
}
unsigned short ArgMnp::get(int i, unsigned short& r)
{
	r = sqlite3_column_int(m_stmt, i);
	return r;
}
int ArgMnp::get(int i, int& r)
{
	r = sqlite3_column_int(m_stmt, i);
	return r;
}
unsigned int ArgMnp::get(int i, unsigned int& r)
{
	r = sqlite3_column_int(m_stmt, i);
	return r;
}
long ArgMnp::get(int i, long& r)
{
	r = sqlite3_column_int(m_stmt, i);
	return r;
}
unsigned long ArgMnp::get(int i, unsigned long& r)
{
	r = sqlite3_column_int(m_stmt, i);
	return r;
}
sqlite3_int64 ArgMnp::get(int i, sqlite3_int64& r)
{
	r = sqlite3_column_int64(m_stmt, i);
	return r;
}
sqlite3_uint64 ArgMnp::get(int i, sqlite3_uint64& r)
{
	r = sqlite3_column_int64(m_stmt, i);
	return r;
}
float ArgMnp::get(int i, float& r)
{
	r = (float)sqlite3_column_double(m_stmt, i);
	return r;
}
double ArgMnp::get(int i, double& r)
{
	r = sqlite3_column_double(m_stmt, i);
	return r;
}
cstr ArgMnp::get(int i, cstr& r)
{
	r = (cstr)sqlite3_column_text(m_stmt, i);
	return r;
}
cmem ArgMnp::get(int i, cmem& r)
{
	r = sqlite3_column_blob(m_stmt, i);
	return r;
}
string ArgMnp::get(int i, string& r)
{
	r = (cstr)sqlite3_column_text(m_stmt, i);
	return r;
}
astr ArgMnp::cpy(int i, astr* p)
{
	const astr tmp = (const astr)sqlite3_column_text(m_stmt, i);
	int len = sqlite3_column_bytes(m_stmt, i) + 1;
	astr v = strncpy((char*)malloc(len), tmp, len);
	if (p)
		*p = v;
	return v;
}
amem ArgMnp::cpy(int i, amem* p)
{
	cmem tmp = sqlite3_column_blob(m_stmt, i);
	int len = sqlite3_column_bytes(m_stmt, i);
	amem v = memcpy(malloc(len), tmp, len);
	if (p)
		*p = v;
	return v;
}
astr ArgMnp::prt(int i, astr& r)
{
	const char* tmp = (const char*)sqlite3_column_text(m_stmt, i);
	if (r)
		strcpy(r, tmp);
	return r;
}
amem ArgMnp::prt(int i, amem& r)
{
	const void* tmp = sqlite3_column_blob(m_stmt, i);
	int len = sqlite3_column_bytes(m_stmt, i);
	if (r)
		memcpy(r, tmp, len);
	return r;
}
