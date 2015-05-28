#ifndef _GWAS_CAST_RESOLVER_H_
#define _GWAS_CAST_RESOLVER_H_
#include <cstdio>
#include <type_traits>
namespace cast
{
	/* Base class for Solve classes.
	T -- original composed type
	D -- derived Solve<T...> types, Curiously Recurring Template Pattern
	U -- underlying type, for simple type and references, U=T
	N -- array extent, for simple type and references, N=1	*/
	template<typename T, typename D, typename U = T, size_t N = 1>
	struct Org
	{
		// common definition
		Org() = delete;
		typename typedef T org;					// original composed type.
		typename typedef U udr;					// underlying type.
		static const size_t u = sizeof(U);		// unit sizeof underlying type.
		static const size_t n = N;				// unit count.
		static void print(FILE* out = stdout)
		{
			if (out)
				fprintf(out, "%p %-24s%d*%d\t%p %s\n", &typeid(D::dcy), typeid(D::dcy).name(), u, n, &typeid(org), typeid(org).name());
		}

		/* The following must be defined by specific Solve<T...> class derived from this base.*/
		//typename typedef T dcy;				// decayed type of T
		//typename typedef T& ref;				// static_cast<ref>(T) must compile and get reference type of T.
		//typename typedef const T& crf;		// static_cast<crf>(T) must compile and get const reference type of T.

	};

	/* ----- raw type ----- */
	template<class T>
	struct Solve :public Org<T, Solve<T>>
	{
		typename typedef T dcy;
		typename typedef const T& crf;
		typename typedef T& ref;
		static void tag_()
		{
			print();
		}
	};

	template<class T>
	struct Solve<const T> :public Org<const T, Solve<const T>>
	{
		typename typedef T dcy;
		typename typedef const T& crf;
		typename typedef T& ref;
		static void tag_const()
		{
			print();
		}
	};

	/* ----- arrays ----- */
	template<class T, size_t N>
	struct Solve<T[N]> :public Org<T[N], Solve<T[N]>, T, N>
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_ary_N()
		{
			print();
		}
	};
	template<class T>
	struct Solve<T[]> :public Org<T[], Solve<T[]>, T, 0>
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_ary_X()
		{
			print();
		}
	};
	template<class T, size_t N>
	struct Solve<const T[N]> :public Org<const T[N], Solve<const T[N]>, T, N>
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_const_ary_N()	// const array of size N
		{
			print();
		}
	};
	template<class T>
	struct Solve<const T[]> :public Org<const T[], Solve<const T[]>, T, 0>
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_const_ary_X()	// const array unknown size
		{
			print();
		}
	};

	/* ----- arrays reference ----- */
	template<class T, size_t N>
	struct Solve<T(&)[N]> :public Org<T(&)[N], Solve<T(&)[N]>, T, N>		// Solve const array reference. Unnamed r-value is of this dcy. e.g. decltype("cstr").
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_ref_ary_N()	// const solve array
		{
			print();
		}
	};
	template<class T, size_t N>
	struct Solve<const T(&)[N]> :public Org<const T(&)[N], Solve<const T(&)[N]>, T, N>		// Solve const array reference. Unnamed r-value is of this dcy. e.g. decltype("cstr").
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_const_ref_ary_N()	// const solve array
		{
			print();
		}
	};
	template<class T>
	struct Solve<T(&)[]> :public Org<T(&)[], Solve<T(&)[]>, T, 0>		// Solve const array reference. Unnamed r-value is of this dcy. e.g. decltype("cstr").
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_ref_ary_X()	// const solve array
		{
			print();
		}
	};
	template<class T>
	struct Solve<const T(&)[]> :public Org<const T(&)[], Solve<const T(&)[]>, T, 0>		// Solve const array reference. Unnamed r-value is of this dcy. e.g. decltype("cstr").
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_ref_ary_X()	// const solve array
		{
			print();
		}
	};

	/* ----- reference ----- */
	template<class T>
	struct Solve<T&> :public Org<T&, Solve<T&>>
	{
		typename typedef T dcy;
		typename typedef const T& crf;
		typename typedef T& ref;
		static void tag_ref()	// const solve array
		{
			print();
		}
	};
	template<class T>
	struct Solve<const T&> :public Org<const T&, Solve<const T&>>
	{
		typename typedef T dcy;
		typename typedef const T& crf;		// const reference dcy
		typename typedef T& ref;
		static void tag_const_ref()
		{
			print();
		}
	};

	/* ----- pointer ----- */
	template<class T>
	struct Solve<T*> :public Org<T*, Solve<T*>, T, 0>
	{
		typename typedef T* dcy;			// exit for recursive pointer dcy solving.
		typename typedef T* const crf;
		typename typedef T* ref;
		static void tag_ptr()
		{
			print();
		}
	};
	template<class T>
	struct Solve<const T*> :public Org<const T*, Solve<const T*>, T, 0>
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
		static void tag_const_ptr()
		{
			print();
		}
	};
	template<class T>
	struct Solve<T* const> :public Org<T* const, Solve<T* const>, T, 0>
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
	};
	template<class T>
	struct Solve<const T* const> :public Org<const T* const, Solve<const T* const>, T, 0>
	{
		typename typedef T* dcy;
		typename typedef const T* crf;
		typename typedef T* ref;
	};
}

#endif