#include "Castor3D/Scene/RenderedObject.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	void RenderedObject::fillEntry( uint32_t nodeId
		, Pass const & pass
		, SceneNode const & sceneNode
		, uint32_t meshletCount
		, ModelBufferConfiguration & modelData )
	{
		if ( !sceneNode.isVisible() )
		{
			return;
		}

		auto & modelMtx = sceneNode.getDerivedTransformationMatrix();
		auto normal = castor::Matrix3x3f{ modelMtx };

		modelData.prvModel = m_firstUpdate
			? modelMtx
			: modelData.curModel;
		m_firstUpdate = false;
		modelData.curModel = modelMtx;
		modelData.normal = castor::Matrix4x4f{ normal.getInverse().getTransposed() };

		modelData.materialId = pass.getId();
		modelData.shadowReceiver = isShadowReceiver() ? 1u : 0u;

		if ( pass.hasEnvironmentMapping() )
		{
			modelData.envMapId = sceneNode.getScene()->getEnvironmentMapIndex( sceneNode ) + 1u;
		}

		auto scale = sceneNode.getDerivedScale();
		modelData.scale = scale;
		modelData.meshletCount = meshletCount;
		auto ires = m_modelsDataOffsets.emplace( nodeId, std::make_pair( &modelData, Offsets{} ) );
		auto & offsets = ires.first->second.second;

		if ( offsets.vertexOffset || offsets.indexOffset )
		{
			modelData.indexOffset = uint32_t( offsets.indexOffset );
			modelData.vertexOffset = uint32_t( offsets.vertexOffset );
			modelData.meshletOffset = uint32_t( offsets.meshletOffset );
		}
	}

	void RenderedObject::fillEntryOffsets( uint32_t nodeId
		, VkDeviceSize vertexOffset
		, VkDeviceSize indexOffset
		, VkDeviceSize meshletOffset )
	{
		auto ires = m_modelsDataOffsets.emplace( nodeId, std::make_pair( nullptr, Offsets{} ) );
		auto & dataOffsets = ires.first->second;
		dataOffsets.second.indexOffset = indexOffset;
		dataOffsets.second.vertexOffset = vertexOffset;
		dataOffsets.second.meshletOffset = meshletOffset;

		if ( dataOffsets.first )
		{
			dataOffsets.first->indexOffset = uint32_t( indexOffset );
			dataOffsets.first->vertexOffset = uint32_t( vertexOffset );
			dataOffsets.first->meshletOffset = uint32_t( meshletOffset );
		}
	}
}
