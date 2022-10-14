#include "Castor3D/Material/MaterialModule.hpp"

namespace castor3d
{
	VkFormat convert( castor::PixelFormat format )
	{
		return VkFormat( format );
	}

	castor::PixelFormat convert( VkFormat format )
	{
		return castor::PixelFormat( format );
	}

	bool operator==( TextureCombine const & lhs, TextureCombine const & rhs )
	{
		return lhs.configCount == rhs.configCount
			&& lhs.flags == rhs.flags;
	}
}
