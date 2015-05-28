#ifndef _GWAS_QVEC_H_
#define _GWAS_QVEC_H_

#include <vector>
#include <string>
template<class T> class QVec;
template<typename T, typename ...VALS>
static QVec<T> qvec(T val, VALS...more)
{
	return QVec<T>(val, more);
}
typedef QVec<std::string>	SVec;
typedef QVec<int>			IVec;


/*The Quick Vector is a std::vector wrapping class.
Use Operator () to consequtively add elements.
Shortened method names to save typing.
A much smaller interface is provied.*/
template<class T>
class QVec : public std::vector < T >
{
public:
	typedef typename std::vector<T>::iterator		Itr;	// shortened vector iterator type
	typedef typename std::vector<T>::const_iterator	Ctr;	// shortened vector const iterator type

public:
	template<typename ...VALS>
	QVec(T val, VALS...more)
	{
		operator()(val, more...);
	}
	QVec(size_t reserve) : std::vector<T>(reserve)
	{}
	QVec()
	{}
	/* Add element to the vector. Continiously use operator(...) to add more.
	Use hint: x_vec(1_st)(2_nd)(3_rd)...(n_th);
	Supply an empty argument list to clear the vector.*/
	QVec<T>& operator()(const T& obj)
	{
		std::vector<T>::push_back(obj);
		return *this;
	}
	template<typename... Args>
	QVec<T>& operator()(const T& obj, Args...more)
	{
		std::vector<T>::push_back(obj);
		return operator()(more...);
	}
	QVec<T>& clr()
	{
		std::vector<T>::clear();
		return *this;
	}

	// wrapped std::vector::operator[], take int index instead of likes of __int64 or size_t
	// auto expand vector size to i+1
	T& operator[](size_t i)
	{
		if (i + 1 > std::vector<T>::size())
			std::vector<T>::resize(i + 1);
		return std::vector<T>::operator[](i);
	}
	// wrapped std::vector::operator[], take int index instead of likes of __int64 or size_t
	// auto expand vector size to i+1
	const T& operator[](size_t i) const
	{
		return std::vector<T>::operator[](i);
	}


	Itr p0()		// iterator to the 0 th. element.
	{
		return std::vector<T>::begin();
	}
	Ctr p0() const	// iterator to the 0 th. element.
	{
		return std::vector<T>::cbegin();
	}
	Itr pN()		// iterator to the N th. element.
	{
		return std::vector<T>::end();
	}
	Ctr pN() const	// iterator to the N th. element.
	{
		return std::vector<T>::cend();
	}

	T& pf(T& e)					// push
	{
		std::vector<T>::push_back(e);
		return e;
	}
	const T& psh(const T& e)	// push constant
	{
		std::vector<T>::push_back(e);
		return e;
	}
	T& pop()
	{
		T& e = std::vector<T>::back();
		std::vector<T>::pop_back();
		return e;
	}
	const T& pop() const
	{
		const T& e = std::vector<T>::back();
		std::vector<T>::pop_back();
		return e;
	}
	/*Number of elements.
	The quick vector is never meant hold too many data, a int return type instead of size_t
	could avoid compiler warnnings like size_t to int and unsigned to signed cast.*/
	int n() const
	{
		return (int)std::vector<T>::size();
	}
};
#endif