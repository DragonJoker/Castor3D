#include "Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp"

CU_ImplementSmartPtr( castor3d, IndexMapping )

namespace castor3d
{
	IndexMapping::IndexMapping( Submesh & submesh
		, castor::String const & type
		, SubmeshComponentDataUPtr data )
		: SubmeshComponent{ submesh, type, castor::move( data ) }
	{
	}
}
