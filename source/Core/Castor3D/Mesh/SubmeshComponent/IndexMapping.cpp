#include "IndexMapping.hpp"

using namespace castor;

namespace castor3d
{
	IndexMapping::IndexMapping( Submesh & submesh
		, String const & type )
		: SubmeshComponent{ submesh, type }
	{
	}

	IndexMapping::~IndexMapping()
	{
	}
}
