#include "Aria/Database/DbField.hpp"

namespace aria::db
{
	Field::Field( Connection & connection, ValuedObjectInfos infos )
		: ValuedObject{ connection, std::move( infos ) }
	{
		doCreateValue();
	}
}
