#include <string>
#include <map>

class NamedPropertyBase
{
public:
	std::string name;		//not necessary, but cute. depends on the thing we're doing.
	virtual void Function() {};
};


template <typename T>
class TypedProperty : public NamedPropertyBase
{
public:

	T m_value;
};