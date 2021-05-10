/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneRenderNodes_H___
#define ___C3D_SceneRenderNodes_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	struct SceneRenderNodes
		: castor::OwnedBy< Scene const >
	{
		C3D_API explicit SceneRenderNodes( Scene const & scene );

		C3D_API void initialiseNodes( RenderDevice const & device );
		C3D_API SubmeshRenderNode & createNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, Submesh & data
			, Geometry & instance
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		C3D_API BillboardRenderNode & createNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, BillboardBase & instance
			, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer );
		C3D_API ashes::DescriptorSetLayoutCRefArray getDescriptorSetLayouts( Pass const & pass
			, BillboardBase const & billboard );
		C3D_API ashes::DescriptorSetLayoutCRefArray getDescriptorSetLayouts( Pass const & pass
			, Submesh const & submesh
			, AnimatedMesh const * mesh
			, AnimatedSkeleton const * skeleton );
		C3D_API void update( GpuUpdater & updater );

		bool hasNodes()const
		{
			return !m_submeshNodes.empty()
				|| !m_billboardNodes.empty();
		}

	private:
		ashes::DescriptorSetLayoutCRefArray doGetDescriptorSetLayouts( Pass const & pass
			, BillboardBase const * billboard
			, Submesh const * submesh
			, AnimatedMesh const * mesh
			, AnimatedSkeleton const * skeleton );;
		void doUpdateDescriptorsCounts( Pass const & pass
			, BillboardBase * billboard
			, Submesh const * submesh
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		void doInitialiseDescriptorPool( RenderDevice const & device );
		void doUpdateNode( SubmeshRenderNode & node );
		void doUpdateNode( BillboardRenderNode & node );

	public:
		struct DescriptorSetLayouts
		{
			ashes::DescriptorSetLayoutPtr ubo;
			ashes::DescriptorSetLayoutPtr tex;
		};

	private:
		std::unordered_map< size_t, SubmeshRenderNodeUPtr > m_submeshNodes;
		std::unordered_map< size_t, BillboardRenderNodeUPtr > m_billboardNodes;
		std::unordered_map< size_t, DescriptorSetLayouts > m_descriptorLayouts;
		uint32_t m_currentPoolSize{};
		struct DescriptorCounts
		{
			uint32_t uniformBuffers{};
			uint32_t storageBuffers{};
			uint32_t samplers{};
			uint32_t texelBuffers{};
		};
		DescriptorCounts m_descriptorCounts;
		ashes::DescriptorPoolPtr m_descriptorPool;
	};
}

#endif
