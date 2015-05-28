#if (defined(_WIN32) || defined(WIN32)||defined(_MSC_VER)) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "dbms.h"
#include <exp.h>
#include "task/task.h"

#ifndef ASYNC_IO
#define ASYNC_IO 0
#endif

#if ASYNC_IO > 0
#include <thread>
std::thread& _io = std::thread();
#endif

// the one and only sqlite database instance.
sqlite3* _me = 0;

void db::open()
{
	int rc;
	const char* vfs = 0;

#if ASYNC_IO > 0
	// register sqlite3 asynchrozied VFC
	rc = sqlite3async_initialize(NULL, 0);
	if (SQLITE_OK != rc)
		GEXP(db::exp_db() << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(_me)));
	vfs = SQLITEASYNC_VFSNAME;
	//Start sqlite3_asyn background disk IO thread.
	_io = std::thread(sqlite3async_run);
#endif

	// initialize dbms as memory db.
	rc = sqlite3_open_v2(":memory:", &_me, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, vfs);
	if (SQLITE_OK != rc)
		GEXP(db::exp_db() << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(_me)));

	// attach harddisk mirror db.
	sqlite3_stmt* s;
	rc = sqlite3_prepare_v2(_me, "ATTACH gws_db AS "HDB, -1, &s, 0);
	if (SQLITE_OK != rc)
		GEXP(db::exp_db() << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(_me)));
	rc = sqlite3_step(s);
	if (SQLITE_DONE != rc)
		GEXP(db::exp_db() << errn(rc) << errs(sqlite3_errstr(rc)) << errm(sqlite3_errmsg(_me)));
	sqlite3_finalize(s);
}
void db::shut()
{
	sqlite3_close_v2(_me);
#if ASYNC_IO > 0
	printf("waiting io thread to finish.\n");
	sqlite3async_control(SQLITEASYNC_HALT, SQLITEASYNC_HALT_IDLE);
	_io.join();	// wait until _io thread ends.
	sqlite3async_shutdown();
#endif
}

sqlite3* db::me()
{
	return _me;
}

template<typename T> const std::string db::Cast<T>::NAME = "";
const std::string db::Cast<char>::NAME = "INT";
const std::string db::Cast<unsigned char>::NAME = "INT";
const std::string db::Cast<short>::NAME = "INT";
const std::string db::Cast<unsigned short>::NAME = "INT";
const std::string db::Cast<int>::NAME = "INT";
const std::string db::Cast<unsigned int>::NAME = "INT";
const std::string db::Cast<long>::NAME = "INT";
const std::string db::Cast<unsigned long>::NAME = "INT";
const std::string db::Cast<sqlite3_int64>::NAME = "INT";
const std::string db::Cast<sqlite3_uint64>::NAME = "INT";
const std::string db::Cast <float>::NAME = "FLOAT";
const std::string db::Cast <double>::NAME = "FLOAT";
const std::string db::Cast <astr>::NAME = "TEXT";
const std::string db::Cast <cstr>::NAME = "TEXT";
const std::string db::Cast <amem>::NAME = "BLOB";
const std::string db::Cast <cmem>::NAME = "BLOB";
const std::string db::Cast <std::string>::NAME = "TEXT";
