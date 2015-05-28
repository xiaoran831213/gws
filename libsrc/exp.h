#ifndef _GWAS_EXP_H_
#define _GWAS_EXP_H_

#include <boost/exception/all.hpp>
#include <string>
#include "typedefs.h"

#define __CLZZ__	(typeid(*this).name())
#ifdef _MSC_VER
#define __FUNC__	__FUNCTION__
#endif

#define GEXP(x)	BOOST_THROW_EXCEPTION(x)
#define JEXP_INIT(err_id, err_type)	JmpExp<err_type> err_id
#define JEXP(jump_label,err_id,...)	{err_id.thrw(__FUNC__,__FILE__,__LINE__,__VA_ARGS__);goto jump_label;}
#define JEXP_CHCK(err_id)			if(err_id.line) boost::exception_detail::throw_exception_(err_id.eobj, err_id.func, err_id.file, err_id.line);

typedef boost::error_info<struct _clib, std::string>		clib;	// error c library
typedef boost::error_info<struct _cfun, std::string>		cfun;	// error c function
typedef boost::error_info<struct _errs, std::string>		errs;	// c library error string.
typedef boost::error_info<struct _errn, std::size_t>		errn;	// c library error number.
typedef boost::error_info<struct _errm, std::string>		errm;	// error message.
typedef boost::error_info<struct _clzz, std::string>		clzz;	// class
typedef boost::error_info<struct _name, std::string>		name;	// name
typedef boost::error_info<struct _path, std::string>		path;	// path
typedef boost::error_info<struct _size, std::size_t>		size;	// size
typedef boost::error_info<struct _mode, std::string>		mode;	// mode
typedef boost::error_info<struct _type, std::string>		type;	// type
typedef boost::error_info<struct _desc, std::string>		desc;	// description

struct exp_base : virtual boost::exception, virtual std::exception { };
struct exp_capi : virtual exp_base{};	//c api exception.
struct exp_file : virtual exp_base{};
struct exp_null : virtual exp_base{};

template<typename E>
struct JmpExp
{
	typedef typename boost::exception_detail::enable_error_info_return_type<E>::type type;
	type eobj;
	uint line = 0;
	cstr func = 0;
	cstr file = 0;
	template<typename ...Args>
	void thrw(cstr func, cstr file, uint line, Args... more)
	{
		this->line = line;
		this->func = func;
		this->file = file;
		this->more(more...);
	}
	template<typename Tag, typename Vtp, typename ...Args>
	void more(boost::error_info<Tag, Vtp> const &info, Args... rest)
	{
		boost::exception_detail::set_info(eobj, info);
		more(rest...);
	}
	template <typename T, typename ...Args>
	void more(T const &skip, Args... rest)
	{
		more(rest...);
	}
	void more()
	{}
};

#endif
