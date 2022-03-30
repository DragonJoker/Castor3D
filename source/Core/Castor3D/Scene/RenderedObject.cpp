#include "Castor3D/Scene/RenderedObject.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	void RenderedObject::fillEntry( Pass const & pass
		, SceneNode const & sceneNode
		, ModelBufferConfiguration & modelData )
	{
		auto & modelMtx = sceneNode.getDerivedTransformationMatrix();
		auto normal = castor::Matrix3x3f{ modelMtx };

		modelData.prvModel = modelData.curModel;
		modelData.curModel = modelMtx;
		modelData.normal = castor::Matrix4x4f{ normal.getInverse().getTransposed() };

		uint32_t index = 0u;
		modelData.textures[0] = {};
		modelData.textures[1] = {};

		for ( auto & unit : pass )
		{
			modelData.textures[index / 4u][index % 4u] = unit->getId();
			++index;
		}

		modelData.countsIDs->x = int32_t( std::min( 8u, pass.getTextureUnitsCount() ) );
		modelData.countsIDs->y = int( pass.getId() );
		modelData.countsIDs->w = isShadowReceiver();

		if ( pass.hasEnvironmentMapping() )
		{
			modelData.countsIDs->z = int( sceneNode.getScene()->getEnvironmentMapIndex( sceneNode ) + 1u );
		}
	}
}
