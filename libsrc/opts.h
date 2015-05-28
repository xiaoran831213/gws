#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#define OP_SAVE			"save_cfg"
#define OP_LOAD			"load_cfg"

#define VERSION			"0.5"
#include <tclap/CmdLine.h>
#include <qvec.h>

typedef TCLAP::CmdLine CmdPsr;						// command line parser type

template<typename T>
struct CmdArg
{
	typename typedef TCLAP::ValueArg<T>					Val;		// single value argument type
	typename typedef TCLAP::MultiArg<T>					Mlt;		// multiple value argument type
	typename typedef TCLAP::UnlabeledValueArg<T>		Ulb;		// unlabled value argument type
	typename typedef TCLAP::UnlabeledMultiArg<T>		Rst;		// multiple unlabled argument type, represent rest of all unparsed, unlabled arguments
	typename typedef TCLAP::MultiArg<T>::const_iterator	Ctr;		// const iterator type of multiple value argument
	class Cst : public TCLAP::ValuesConstraint<T>
	{
	public:
		Cst() :ValuesConstraint()
		{}
		Cst(std::vector<T> opts) : ValuesConstraint(opts)
		{}
		template<typename ...Args>
		Cst(T opt1, Args ...rest) : ValuesConstraint(QVec<T>(opt1, rest...))
		{}
	};
};

template<>
struct CmdArg < bool >
{
	typedef TCLAP::SwitchArg					Swt;		// switch argument type
	typedef TCLAP::MultiSwitchArg				MltSwt;		// multiple switch value argument type
};


#endif
