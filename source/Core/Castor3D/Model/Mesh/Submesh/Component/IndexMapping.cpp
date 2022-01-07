#include "Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp"

using namespace castor;

namespace castor3d
{
	IndexMapping::IndexMapping( Submesh & submesh
		, String const & type
		, VkMemoryPropertyFlags bufferMemoryFlags
		, VkBufferUsageFlags bufferUsageFlags )
		: SubmeshComponent{ submesh, type, uint32_t( std::hash< std::string >{}( type ) ) }
		, m_bufferMemoryFlags{ bufferMemoryFlags }
		, m_bufferUsageFlags{ bufferUsageFlags }
	{
	}
}
