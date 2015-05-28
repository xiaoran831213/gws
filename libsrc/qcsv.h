#if (defined(_WIN32) || defined(WIN32)||defined(_MSC_VER))
#pragma warning( disable : 4996)
#endif

#ifndef _GWAS_MIST_QCSV_H_
#define _GWAS_MIST_QCSV_H_

#include <string>
#include <vector>
#include "arg.h"
#include "libcsv.h"

#include "typedefs.h"

class QCsv
{
public:
	// dynamic arguments
	typedef ARG<struct tag_qcsv_sept_chr, char>		SEP;	// separater char
	typedef ARG<struct tag_qcsv_sept_chr, FILE*>	SNK;	// output file
protected:
	static void qcsv_proc_word_callback(void* s, sztp len, void* data)
	{
		QCsv* p = (QCsv*)data;
		p->m_wptr = (astr)s;		// update word pointer
		p->m_wlen = len;			// update word length
		p->on_word();				// process new word
		p->m_icol++;				// update col index
	}
	static void qcsv_proc_line_callback(int c, void* data)
	{
		QCsv* p = (QCsv*)data;
		p->on_line();
		p->m_irow++;
		p->m_icol = 0;
	}
	static void qcsv_scan_word_callback(void* s, sztp len, void* data)
	{
		QCsv* p = (QCsv*)data;
		p->m_icol++;			// update col index
	}
	static void qcsv_scan_line_callback(int c, void* data)
	{
		QCsv* p = (QCsv*)data;
		p->m_irow++;
		p->m_icol = 0;
	}
protected:
	const std::string m_estr = std::string();	// the empty string
	/* ----- configuration*/
	FILE*						m_file = 0;		// source file pointer
	FILE*						m_fsnk = 0;		// sink file pointer
	char						m_sptr = ',';	// delimiter character.

	/* ----- CSV parsing context -----*/
	sztp						m_icol = 0;	// index of current col
	sztp						m_irow = 0;	// index of current row
	sztp						m_ncol = 0;	// number of cols.
	sztp						m_nrow = 0;	// number of rows.
	astr						m_wptr = 0;	// word pointer
	sztp						m_wlen = 0;	// word length
	/* ----- helper members ----- */
public:
	// process next dynamic argument, then process the rest
	template<typename ...Args>
	QCsv(FILE* file, Args...rest) : QCsv(rest...)
	{
		m_file = file;
	}
	virtual ~QCsv()
	{
	}

private:
	template<typename ...Args>
	QCsv(SEP arg, Args...rest) : QCsv(rest...)
	{
		m_sptr = arg.val();
	}
	template<typename ...Args>
	QCsv(SNK arg, Args...rest) : QCsv(rest...)
	{
		m_fsnk = arg.val();
	}
	template<typename Tag, typename Vtp, typename ...Args>
	QCsv(ARG<Tag, Vtp> skip, Args...rest) : QCsv(rest)
	{
		std::cout << "skiped argument: " << skip.key() << " = " << skip.val() << std::endl;
	}
	QCsv()	//exit of the recursive constructor call
	{
	}
public:
	inline char sptr() const	// the word separator
	{
		return m_sptr;
	}
	inline sztp icol() const		// context: col index
	{
		return m_icol;
	}
	inline sztp irow() const		// context: row index
	{
		return m_irow;
	}
	inline sztp ncol() const		// number of cols
	{
		return m_ncol;
	}
	inline sztp nrow() const		// number of rows
	{
		return m_nrow;
	}
	inline astr wptr() const		// context: word pointer
	{
		return m_wptr;
	}
	inline sztp wlen() const		// context: word length
	{
		return m_wlen;
	}

public:
	virtual void proc();
	virtual void scan();

protected:
	virtual void on_word(); // on regular word feed
	virtual void on_line(); // on regular line feed
	virtual void on_init(); // before everything
	virtual void on_head(); // finish of header
	virtual void on_fini(); // finish of file
	virtual void on_ckey(); // on col key (words in the head)
	virtual void on_rkey(); // on row key (1st word of a row)

	virtual bool is_null(cstr word, sztp wlen)				// override to provide null word check
	{
		return false;
	}
	virtual bool is_cmmt(cstr word, sztp wlen)				// override to provide comment check
	{
		return false;
	}
};

class ZCsv : public QCsv
{
public:
	template<typename ...Args>
	ZCsv(FILE* file, Args...rest) : QCsv(file, rest...)
	{}
	virtual ~ZCsv()
	{}
	virtual void proc() override;
protected:
	virtual void on_word()	override;
	virtual void on_line()	override;
};

#endif
