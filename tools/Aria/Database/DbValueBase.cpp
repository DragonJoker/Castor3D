#include "Aria/Database/DbValueBase.hpp"

namespace aria::db
{
	ValueBase::ValueBase( Connection & connection )
		: m_connection{ connection }
	{
	}
}
