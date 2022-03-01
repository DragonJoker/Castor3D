/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneRenderNodes_H___
#define ___C3D_SceneRenderNodes_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

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
	public:
		template< typename NodeT >
		using DescriptorNodesPtrT = std::unordered_map< size_t, castor::UniquePtr< NodeT > >;
		template< typename NodeT >
		using DescriptorNodesPoolsT = std::unordered_map< size_t, DescriptorNodesPtrT< NodeT > >;

	public:
		C3D_API explicit SceneRenderNodes( Scene const & scene );

		C3D_API void clear();
		C3D_API SubmeshRenderNode & createNode( Pass & pass
			, Submesh & data
			, Geometry & instance
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		C3D_API BillboardRenderNode & createNode( Pass & pass
			, BillboardBase & instance );
		C3D_API void update( CpuUpdater & updater );
		C3D_API void update( GpuUpdater & updater );

		bool hasNodes()const
		{
			return !m_submeshNodes.empty()
				|| !m_billboardNodes.empty();
		}

		ashes::Buffer< ModelBufferConfiguration > const & getModelBuffer()const
		{
			return *m_nodesData;
		}

		ashes::Buffer< BillboardUboConfiguration > const & getBillboardsBuffer()const
		{
			return *m_billboardsData;
		}

		DescriptorNodesPoolsT< SubmeshRenderNode > const & getSubmeshNodes()const
		{
			return m_submeshNodes;
		}

		DescriptorNodesPoolsT< BillboardRenderNode > const & getBillboardNodes()const
		{
			return m_billboardNodes;
		}

	private:
		void doUpdateNode( SubmeshRenderNode & node
			, ModelBufferConfiguration * modelBufferData );
		void doUpdateNode( BillboardRenderNode & node
			, ModelBufferConfiguration * modelBufferData
			, BillboardUboConfiguration * billboardBufferData );

	private:
		RenderDevice const & m_device;
		std::mutex m_nodesMutex;
		DescriptorNodesPoolsT< SubmeshRenderNode > m_submeshNodes;
		DescriptorNodesPoolsT< BillboardRenderNode > m_billboardNodes;
		ashes::BufferPtr< ModelBufferConfiguration > m_nodesData;
		ashes::BufferPtr< BillboardUboConfiguration >m_billboardsData;
		uint32_t m_nodeId{};
		bool m_dirty{ true };
	};
}

#endif
