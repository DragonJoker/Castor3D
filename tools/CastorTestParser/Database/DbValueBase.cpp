#include "CastorTestParser/Database/DbValueBase.hpp"

namespace test_parser::db
{
	ValueBase::ValueBase( Connection & connection )
		: m_connection{ connection }
	{
	}
}
