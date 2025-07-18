#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"

#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d::shader
{
	sdw::Vec3 BillboardData::getCameraRight( CameraData const & cameraData )const
	{
		return getWriter()->ternary( isSpherical() != 0_u
			, normalize( cameraData.getCurViewRight() )
			, normalize( vec3( cameraData.curView()[0][0], 0.0_f, cameraData.curView()[2][0] ) ) );
	}

	sdw::Vec3 BillboardData::getCameraUp( CameraData const & cameraData )const
	{
		return getWriter()->ternary( isSpherical() != 0_u
			, normalize( cameraData.getCurViewUp() )
			, vec3( 0.0_f, 1.0_f, 0.0_f ) );
	}

	sdw::Float BillboardData::getWidth( RenderData const & renderData )const
	{
		return getWriter()->ternary( isFixedSize() != 0_u
			, dimensions().x() * renderData.invRenderSize().x()
			, dimensions().x() );
	}

	sdw::Float BillboardData::getHeight( RenderData const & renderData )const
	{
		return getWriter()->ternary( isFixedSize() != 0_u
			, dimensions().y() * renderData.invRenderSize().y()
			, dimensions().y() );
	}
}
