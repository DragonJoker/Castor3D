#include "Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp"

namespace castor3d
{
	IndexMapping::IndexMapping( Submesh & submesh
		, castor::String const & type
		, VkBufferUsageFlags bufferUsageFlags )
		: SubmeshComponent{ submesh, type, uint32_t( std::hash< std::string >{}( type ) ) }
		, m_bufferUsageFlags{ bufferUsageFlags }
	{
	}
}
