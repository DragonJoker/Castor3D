#include "Castor3D/Castor3DModule.hpp"

#include "Castor3D/Engine.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <RenderGraph/FramePassTimer.hpp>

CU_ImplementExportedOwnedBy( castor3d::Engine, Engine )
CU_ImplementSmartPtr( castor3d, FramePassTimer )

namespace castor3d
{
	castor::LoggerInstance & getLogger( Engine const & engine )
	{
		return engine.getLogger();
	}

	castor::Matrix4x4f convert( std::array< float, 16 > const & value )
	{
		castor::Matrix4x4f result;
		std::memcpy( result.ptr(), value.data(), 16 * sizeof( float ) );
		return result;
	}

	VkClearColorValue convert( castor::RgbaColour const & value )
	{
		VkClearColorValue result;
		result.float32[0] = value.red();
		result.float32[1] = value.green();
		result.float32[2] = value.blue();
		result.float32[3] = value.alpha();
		return result;
	}

	castor::RgbaColour convert( VkClearColorValue const & value )
	{
		castor::RgbaColour result;
		result.red() = value.float32[0];
		result.green() = value.float32[1];
		result.blue() = value.float32[2];
		result.alpha() = value.float32[3];
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
