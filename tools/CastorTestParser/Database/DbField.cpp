#include "CastorTestParser/Database/DbField.hpp"

namespace test_parser::db
{
	Field::Field( Connection & connection, ValuedObjectInfos infos )
		: ValuedObject{ connection, std::move( infos ) }
	{
		doCreateValue();
	}
}
