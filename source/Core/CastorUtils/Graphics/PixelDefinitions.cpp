#include "CastorUtils/Graphics/PixelDefinitions.hpp"

namespace c3d
{
	String getFormatName( PixelFormat format )
	{
		return makeString( getName( format ) );
	}
}
