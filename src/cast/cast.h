#ifndef _GWAS_MIST_TYPE_CAST_
#define _GWAS_MIST_TYPE_CAST_

#ifdef _MSC_VER
#pragma warning( disable : 4996 )	// disable MSVC deprecation warnning, equivalent to _CRT_SECURE_NO_WARNINGS
#endif

#include "cast_help.h"
#include <typedefs.h>
#include <cstdio>
#include <string>

#define SHOW_VARB(v)	printf("%p %-24s%d\t"#v"\n", &typeid(decltype(v)), typeid(decltype(v)).name(), sizeof(decltype(v)))
#define SHOW_TYPE(...)	printf("%p %-24s%d\t"#__VA_ARGS__"\n", &typeid(__VA_ARGS__), typeid(__VA_ARGS__).name(), sizeof(__VA_ARGS__))
#define TYPE_NAME(...)	(typeid(__VA_ARGS__).name())

namespace cast
{
	void test();
	template<typename S, typename D>
	struct Cast_Basic
	{
		virtual ~Cast_Basic() {}
		virtual int operator()(const S& src, D& dst) const = 0;
	};
	template<typename S, typename M, typename D>
	struct Cast_Indirect : public Cast_Basic<S, D>
	{
		virtual ~Cast_Indirect() {}
		virtual D& operator()(const S& src, D& dst) const override
		{
			M m;
			Cast<S, M>()(src, m);
			Cast<M, D>()(m, dst);
			return 0;
		}
	};
	template<typename S, typename D>
	struct Cast_Indirect<S, astr, D> : public Cast_Basic<S, D>	// specialization for c string indermidiated indirect cast.
	{
		virtual ~Cast_Indirect() {}
		virtual int operator()(const S& src, D& dst) const override
		{
			astr m = (astr)malloc(1024);
			Cast<S, astr, Solve<S>::n, 1024>()(src, m);
			Cast<astr, D, 1024, Solve<D>::n>()(m, dst);
			free(m);
			return 0;
		}
	};

	/* Cast support compile time test.*/
	template<typename C1, typename C2> struct Support { typedef int FOUND; };
	template<typename C1> struct Support<C1, C1>	{};

	template<typename S, typename D, size_t NS = Solve<S>::n, size_t ND = Solve<D>::n>
	struct Cast :virtual public Cast_Basic<S, D>
	{
	public:
		virtual ~Cast() {}
		int operator()(const S& src, D& dst) const override
		{
			typedef Solve<S> I_S;	// source type meta
			typedef Solve<D> I_D;	// target type meta
			typedef Cast<I_S::dcy, I_D::dcy, NS, ND> NEXT;

			//compile time check, fail if the next caster is the same type as current, which means
			//no suitable caster could be FOUND even if S and T is decayed.
			Support <NEXT, Cast<S, D, NS, ND>>::FOUND OK = 1;

			I_S::crf s = static_cast<I_S::crf>(src);
			I_D::ref d = static_cast<I_D::ref>(dst);	// the dst reference must be preserved to ensure proper reception
			NEXT next = NEXT();
			next(I_S::ref(s), d);
			return 0;
		}
	};
	typedef short		int4;
	typedef int			int5;
	typedef long		int6;
	typedef float		flt5;
	typedef double		flt6;
	typedef std::string	stds;

#define DEFN_CAST(S,D) \
	template<size_t NS, size_t ND> struct Cast<S, D, NS, ND> : virtual public Cast_Basic<S, D>\
	{\
	virtual ~Cast() {}\
	int operator()(S const& src, D& dst) const override; \
	}; \
	template<size_t NS, size_t ND> int Cast<S, D, NS, ND>::operator()(S const& src, D& dst) const

	DEFN_CAST(astr, bool)
	{
		const int WS = 16;
		char tStr[][WS] = { "t", "true", "y", "yes", "1" };
		char fStr[][WS] = { "f", "false", "n", "no", "0" };
		char temp[WS];
		strncpy(temp, src, sizeof(temp));
		for (char* p = temp; *p; p++)
		{
			*p = tolower(*p);
			p++;
		}
		for (const char(*pp)[WS] = tStr, (*ee)[WS] = tStr + sizeof(tStr) / WS; pp < ee; pp++)
		{
			if (strcmp(*pp, temp) == 0)	// arrow partial match
			{
				dst = true;
				return 0;
			}
		}
		for (const char(*pp)[WS] = fStr, (*ee)[WS] = fStr + sizeof(fStr) / WS; pp < ee; pp++)
		{
			if (strcmp(*pp, temp) == 0)	// arrow partial match
			{
				dst = false;
				return 0;
			}
		}
		return 1;
	}
	DEFN_CAST(astr, char)
	{
		return sscanf(src, "%c", &dst) == 1 ? 0 : -1;
	}
	DEFN_CAST(astr, int4)
	{
		if (tolower(src[0]) == 'o' || *src == '0' && isdigit(src[1]))
			return sscanf(src + 1, "%ho", &dst) == 1 ? 0 : -1;
		else if (tolower(src[0]) == 'x')
			return sscanf(src + 1, "%hx", &dst) == 1 ? 0 : -1;
		else if (src[0] == '0' && tolower(src[1]) == 'x')
			return sscanf(src + 2, "%hx", &dst) == 1 ? 0 : -1;
		else
			return sscanf(src + 0, "%hd", &dst) == 1 ? 0 : -1;
	}
	DEFN_CAST(astr, int5)
	{
		if (tolower(src[0]) == 'o' || src[0] == '0' && isdigit(src[1]))
			return sscanf(src + 1, "%o", &dst) == 1 ? 0 : -1;
		else if (tolower(src[0]) == 'x')
			return sscanf(src + 1, "%x", &dst) == 1 ? 0 : -1;
		else if (src[0] == '0' && tolower(src[1]) == 'x')
			return sscanf(src + 2, "%x", &dst) == 1 ? 0 : -1;
		else
			return sscanf(src + 0, "%d", &dst) == 1 ? 0 : -1;
	}
	DEFN_CAST(astr, int6)
	{
		if (tolower(src[0]) == 'o' || *src == '0' && isdigit(src[1]))
			return sscanf(src + 1, "%lo", &dst) == 1 ? 0 : -1;
		else if (tolower(src[0]) == 'x')
			return sscanf(src + 1, "%lx", &dst) == 1 ? 0 : -1;
		else if (src[0] == '0' && tolower(src[1]) == 'x')
			return sscanf(src + 2, "%lx", &dst) == 1 ? 0 : -1;
		else
			return sscanf(src + 0, "%ld", &dst) == 1 ? 0 : -1;
	}
	DEFN_CAST(astr, flt5)
	{
		return sscanf(src, "%f", &dst) == 1 ? 0 : -1;
	}
	DEFN_CAST(astr, flt6)
	{
		return sscanf(src, "%Lf", &dst) == 1 ? 0 : -1;
	}
	DEFN_CAST(astr, stds)
	{
		dst = src;
		return 0;
	}

	DEFN_CAST(bool, astr)
	{
		if (ND)
			strncpy(dst, src ? "true" : "false", ND - 1);
		else
			strcpy(dst, src ? "true" : "false");
		return 0;
	}
	DEFN_CAST(char, astr)
	{
		if (ND)
		{
			char buf[32];
			sprintf(buf, "%c", src);
			strncpy(dst, buf, ND - 1);
		}
		else
			sprintf(dst, "%c", src);
		return 0;
	}
	DEFN_CAST(int4, astr)
	{
		if (ND)
		{
			char buf[32];
			sprintf(buf, "%hd", src);
			strncpy(dst, buf, ND - 1);
		}
		else
			sprintf(dst, "%hd", src);
		return 0;
	}
	DEFN_CAST(int5, astr)
	{
		if (ND)
		{
			char buf[32];
			sprintf(buf, "%d", src);
			strncpy(dst, buf, ND - 1);
		}
		else
			sprintf(dst, "%d", src);
		return 0;
	}
	DEFN_CAST(int6, astr)
	{
		if (ND)
		{
			char buf[32];
			sprintf(buf, "%ld", src);
			strncpy(dst, buf, ND - 1);
		}
		else
			sprintf(dst, "%ld", src);
		return 0;
	}
	DEFN_CAST(flt5, astr)
	{
		if (ND)
		{
			char buf[32];
			sprintf(buf, "%f", src);
			strncpy(dst, buf, ND - 1);
		}
		else
			sprintf(dst, "%f", src);
		return 0;
	}
	DEFN_CAST(flt6, astr)
	{
		if (ND)
		{
			char buf[32];
			sprintf(buf, "%Lf", src);
			strncpy(dst, buf, ND - 1)
		}
		else
			sprintf(dst, "%Lf", src);
		return 0;
	}
	DEFN_CAST(stds, astr)
	{
		if (ND)
			strncpy(dst, src.c_str(), ND - 1);
		else
			strncpy(dst, src.c_str());
		return 0;
	}


	template<>
	struct Cast<bool, stds>:virtual public Cast_Indirect<bool, astr, stds>
	{
		virtual ~Cast() {}
	};

}

#endif
