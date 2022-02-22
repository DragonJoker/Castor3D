#include "Castor3D/Cache/CacheModule.hpp"

#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Scene/RenderedObject.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

CU_ImplementCUSmartPtr( castor3d, RenderTargetCache )
CU_ImplementCUSmartPtr( castor3d, ShaderProgramCache )

namespace castor3d
{
	void fillEntry( Pass const & pass
		, SceneNode const & sceneNode
		, RenderedObject const & rendered
		, ModelBufferConfiguration & modelData )
	{
		modelData.prvModel = modelData.curModel;
		modelData.curModel = sceneNode.getDerivedTransformationMatrix();
		auto normal = castor::Matrix3x3f{ modelData.curModel };
		modelData.normal = castor::Matrix4x4f{ normal.getInverse().getTransposed() };
		uint32_t index = 0u;

		for ( auto & unit : pass )
		{
			if ( index < 4 )
			{
				modelData.textures0[index] = unit->getId();
			}
			else if ( index < 8 )
			{
				modelData.textures1[index - 4] = unit->getId();
			}

			++index;
		}

		while ( index < 8u )
		{
			if ( index < 4 )
			{
				modelData.textures0[index] = 0u;
			}
			else
			{
				modelData.textures1[index - 4] = 0u;
			}

			++index;
		}

		modelData.countsIDs->x = int32_t( std::min( 8u, pass.getTextureUnitsCount() ) );
		modelData.countsIDs->y = int( pass.getId() );
		modelData.countsIDs->w = rendered.isShadowReceiver();

		if ( pass.hasEnvironmentMapping() )
		{
			modelData.countsIDs->z = int( sceneNode.getScene()->getEnvironmentMapIndex( sceneNode ) + 1u );
		}
	}
}
