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
		auto modelMtx = sceneNode.getDerivedTransformationMatrix();
		auto normalMtx = castor::Matrix3x3f{ modelMtx }.getInverse().getTransposed();

		if ( !sceneNode.isVisible() )
		{
			modelMtx = {};
			normalMtx = {};
		}

		modelData.prvModel = m_firstUpdate > 0
			? modelMtx
			: modelData.curModel;
		m_firstUpdate = m_firstUpdate ? m_firstUpdate - 1u : 0u;
		modelData.curModel = modelMtx;
		modelData.normal = castor::Matrix4x4f{ normalMtx };

		modelData.materialId = pass.getId();
		modelData.shadowReceiver = isShadowReceiver() ? 1u : 0u;

		if ( pass.hasEnvironmentMapping() )
		{
			modelData.envMapId = sceneNode.getScene()->getEnvironmentMapIndex( sceneNode ) + 1u;
		}

		auto scale = sceneNode.getDerivedScale();
		modelData.scale = scale;
		modelData.meshletCount = meshletCount;
		auto it = m_modelsDataOffsets.try_emplace( nodeId, &modelData, Offsets{} ).first;
		auto const & offsets = it->second.second;

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
		auto & [data, offsets] = m_modelsDataOffsets.try_emplace( nodeId ).first->second;
		offsets.indexOffset = indexOffset;
		offsets.vertexOffset = vertexOffset;
		offsets.meshletOffset = meshletOffset;

		if ( data )
		{
			data->indexOffset = uint32_t( indexOffset );
			data->vertexOffset = uint32_t( vertexOffset );
			data->meshletOffset = uint32_t( meshletOffset );
		}
	}
}
