#include "Castor3D/Castor3DModule.hpp"

#include "Castor3D/Engine.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <RenderGraph/FramePassTimer.hpp>

CU_ImplementExportedOwnedBy( Engine, Engine )
CU_ImplementSmartPtr( c3d, FramePassTimer )

namespace c3d
{
	LoggerInstance & getLogger( Engine const & engine )
	{
		return engine.getLogger();
	}

	Matrix4x4f convert( Array< float, 16 > const & value )
	{
		Matrix4x4f result;
		std::memcpy( result.ptr(), value.data(), 16 * sizeof( float ) );
		return result;
	}

	VkClearColorValue convert( RgbaColour const & value )
	{
		VkClearColorValue result;
		result.float32[0] = value.red();
		result.float32[1] = value.green();
		result.float32[2] = value.blue();
		result.float32[3] = value.alpha();
		return result;
	}

	RgbaColour convert( VkClearColorValue const & value )
	{
		RgbaColour result;
		result.red() = value.float32[0];
		result.green() = value.float32[1];
		result.blue() = value.float32[2];
		result.alpha() = value.float32[3];
		return result;
	}

	Array< float, 4u > makeFloatArray( RgbaColour const & value )
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
