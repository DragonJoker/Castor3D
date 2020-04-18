#include "Castor3D/Castor3DModule.hpp"

#include "Castor3D/Engine.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

CU_ImplementExportedOwnedBy( castor3d::Engine, Engine )

using namespace castor;

namespace castor3d
{
	castor::Matrix4x4f convert( std::array< float, 16 > const & value )
	{
		castor::Matrix4x4f result;
		std::memcpy( result.ptr(), value.data(), 16 * sizeof( float ) );
		return result;
	}

	VkClearColorValue convert( castor::RgbaColour const & value )
	{
		VkClearColorValue result;
		std::memcpy( result.float32, value.constPtr(), 4 * sizeof( float ) );
		return result;
	}

	castor::RgbaColour convert( VkClearColorValue const & value )
	{
		castor::RgbaColour result;
		std::memcpy( result.ptr(), value.float32, 4 * sizeof( float ) );
		return result;
	}

	std::array< float, 4u > makeFloatArray( castor::RgbaColour const & value )
	{
		return
		{
			value.red(),
			value.green(),
			value.blue(),
			value.alpha(),
		};
	}
}
