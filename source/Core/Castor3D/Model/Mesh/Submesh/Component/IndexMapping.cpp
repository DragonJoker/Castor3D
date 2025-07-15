#include "Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp"

CU_ImplementSmartPtr( c3d, IndexMapping )

namespace c3d
{
	IndexMapping::IndexMapping( Submesh & submesh
		, String const & type
		, SubmeshComponentDataUPtr data )
		: SubmeshComponent{ submesh, type, c3d::move( data ) }
	{
	}
}
