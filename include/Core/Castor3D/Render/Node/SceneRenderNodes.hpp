/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneRenderNodes_H___
#define ___C3D_SceneRenderNodes_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <mutex>
#pragma warning( pop )

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

	public:
		struct DescriptorCounts
		{
			DescriptorCounts() = default;
			DescriptorCounts( bool hasSSBO
				, size_t textureCount
				, BillboardBase const * billboard
				, Submesh const * submesh
				, AnimatedMesh const * mesh
				, AnimatedSkeleton const * skeleton );

			void update( bool hasSSBO
				, size_t textureCount
				, BillboardBase const * billboard
				, Submesh const * submesh
				, AnimatedMesh const * mesh
				, AnimatedSkeleton const * skeleton );

			uint32_t uniformBuffers{};
			uint32_t storageBuffers{};
			uint32_t samplers{};
			uint32_t texelBuffers{};
		};

		struct DescriptorSetLayouts
		{
			DescriptorSetLayouts( Engine & engine
				, size_t texturesCount
				, bool instanced
				, BillboardBase const * billboard
				, Submesh const * submesh
				, AnimatedMesh const * mesh
				, AnimatedSkeleton const * skeleton );

			ashes::DescriptorSetLayoutPtr buf;
			ashes::DescriptorSetLayoutPtr tex;
		};

		struct DescriptorSetPools
		{
			DescriptorSetPools( RenderDevice const & device
				, DescriptorCounts const & counts );

			ashes::DescriptorPoolPtr buf;
			ashes::DescriptorPoolPtr tex;
		};

		struct DescriptorPools
		{
			DescriptorPools( DescriptorPools const & ) = delete;
			DescriptorPools & operator=( DescriptorPools const & ) = delete;
			DescriptorPools( DescriptorPools && ) = default;
			DescriptorPools & operator=( DescriptorPools && ) = delete;
			DescriptorPools( Engine & engine
				, DescriptorSetLayouts * layouts
				, DescriptorCounts * counts );

			void allocate( SubmeshRenderNode & node );
			void allocate( BillboardRenderNode & node );

			void deallocate( SubmeshRenderNode & node );
			void deallocate( BillboardRenderNode & node );

		private:
			struct NodeSet
			{
				NodeSet( NodeSet const & ) = delete;
				NodeSet & operator=( NodeSet const & ) = delete;
				NodeSet( NodeSet && ) = default;
				NodeSet & operator=( NodeSet && ) = default;
				NodeSet() = default;

				ashes::DescriptorPool * bufPool;
				ashes::DescriptorSetPtr bufSet;
				ashes::DescriptorPool * texPool;
				ashes::DescriptorSetPtr texSet;
			};

		private:
			void doAllocateBuf( DescriptorSetPools const & pools
				, SubmeshRenderNode & node
				, NodeSet & allocated );
			void doAllocateBuf( DescriptorSetPools const & pools
				, BillboardRenderNode & node
				, NodeSet & allocated );
			void doAllocateTex( DescriptorSetPools const & pools
				, SubmeshRenderNode & node
				, NodeSet & allocated );
			void doAllocateTex( DescriptorSetPools const & pools
				, BillboardRenderNode & node
				, NodeSet & allocated );

		private:
			Engine & m_engine;
			DescriptorSetLayouts * m_layouts;
			DescriptorCounts * m_counts;
			std::vector< DescriptorSetPools > m_pools;
			std::map< intptr_t, NodeSet > m_allocated;
			uint32_t m_available{};
		};

		template< typename NodeT >
		struct DescriptorNodesT
		{
			DescriptorNodesT( Engine & engine
				, size_t texturesCount
				, bool instanced
				, BillboardBase const * billboard
				, Submesh const * submesh
				, AnimatedMesh const * mesh
				, AnimatedSkeleton const * skeleton );
			
			DescriptorCounts counts;
			DescriptorSetLayouts layouts;
			DescriptorPools pools;
			std::unordered_map< size_t, castor::UniquePtr< NodeT > > nodes;
		};

		template< typename NodeT >
		using DescriptorNodesPtrT = std::unique_ptr< DescriptorNodesT< NodeT > >;
		template< typename NodeT >
		using DescriptorNodesPoolsT = std::unordered_map< size_t, DescriptorNodesPtrT< NodeT > >;

	private:
		ashes::DescriptorSetLayoutCRefArray doGetDescriptorSetLayouts( Pass const & pass
			, BillboardBase const * billboard
			, Submesh const * submesh
			, AnimatedMesh const * mesh
			, AnimatedSkeleton const * skeleton );
		void doUpdateDescriptorsCounts( Pass const & pass
			, BillboardBase * billboard
			, Submesh const * submesh
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		void doUpdateNode( SubmeshRenderNode & node );
		void doUpdateNode( BillboardRenderNode & node );

	private:
		std::mutex m_nodesMutex;
		DescriptorNodesPoolsT< SubmeshRenderNode > m_submeshNodes;
		DescriptorNodesPoolsT< BillboardRenderNode > m_billboardNodes;
		DescriptorCounts m_allDescriptorCounts;
	};
}

#endif
