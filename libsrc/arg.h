#ifndef _DYNAMIC_ARGUMENT_H_
#define _DYNAMIC_ARGUMENT_H_

template<typename Tag, typename Vtp>
class ARG
{
private:
	Vtp m_val;
public:
	ARG(const Vtp& val) :m_val(val)
	{}
	const Vtp& val()
	{
		return m_val;
	}
	const Vtp& val(const Vtp& val)
	{
		return m_val = val;
	}
	const char* key()
	{
		return typeid(*this).name();
	}
};
#endif