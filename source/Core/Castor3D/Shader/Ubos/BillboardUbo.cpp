#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"

#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		sdw::Vec3 BillboardData::getCameraRight( MatrixData const & matrixData )const
		{
			return getWriter()->ternary( isSpherical() != 0_u
				, normalize( matrixData.getCurViewRight() )
				, normalize( vec3( matrixData.m_curView[0][0], 0.0_f, matrixData.m_curView[2][0] ) ) );
		}

		sdw::Vec3 BillboardData::getCameraUp( MatrixData const & matrixData )const
		{
			return getWriter()->ternary( isSpherical() != 0_u
				, normalize( matrixData.getCurViewUp() )
				, vec3( 0.0_f, 1.0_f, 0.0_f ) );
		}

		sdw::Float BillboardData::getWidth( SceneData const & sceneData )const
		{
			return getWriter()->ternary( isFixedSize() != 0_u
				, dimensions().x() / sceneData.renderSize().x()
				, dimensions().x() );
		}

		sdw::Float BillboardData::getHeight( SceneData const & sceneData )const
		{
			return getWriter()->ternary( isFixedSize() != 0_u
				, dimensions().y() / sceneData.renderSize().y()
				, dimensions().y() );
		}
	}
}
