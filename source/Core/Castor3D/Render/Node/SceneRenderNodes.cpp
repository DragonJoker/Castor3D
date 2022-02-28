#include "Castor3D/Render/Node/SceneRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>
#include <CastorUtils/Multithreading/MultithreadingModule.hpp>

CU_ImplementCUSmartPtr( castor3d, SceneRenderNodes )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		size_t makeNodeHash( Submesh & data
			, Geometry & instance )
		{
			auto hash = std::hash< Submesh * >{}( &data );
			hash = castor::hashCombinePtr( hash, instance );
			return hash;
		}

		size_t makeNodeHash( BillboardBase & instance )
		{
			auto hash = std::hash< BillboardBase * >{}( &instance );
			return hash;
		}

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

	//*********************************************************************************************

	RenderNodeType getRenderNodeType( ProgramFlags const & flags )
	{
		bool isFrontCulled = checkFlag( flags, ProgramFlag::eInvertNormals );

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			if ( checkFlag( flags, ProgramFlag::eSkinning ) )
			{
				return isFrontCulled
					? RenderNodeType::eFrontInstancedSkinned
					: RenderNodeType::eBackInstancedSkinned;
			}

			return isFrontCulled
				? RenderNodeType::eFrontInstancedStatic
				: RenderNodeType::eBackInstancedStatic;
		}

		if ( checkFlag( flags, ProgramFlag::eMorphing ) )
		{
			return isFrontCulled
				? RenderNodeType::eFrontMorphing
				: RenderNodeType::eBackMorphing;
		}

		if ( checkFlag( flags, ProgramFlag::eSkinning ) )
		{
			return isFrontCulled
				? RenderNodeType::eFrontSkinned
				: RenderNodeType::eBackSkinned;
		}

		if ( checkFlag( flags, ProgramFlag::eBillboards ) )
		{
			return isFrontCulled
				? RenderNodeType::eFrontBillboard
				: RenderNodeType::eBackBillboard;
		}

		return isFrontCulled
			? RenderNodeType::eFrontStatic
			: RenderNodeType::eBackStatic;
	}

	//*********************************************************************************************

	SceneRenderNodes::SceneRenderNodes( Scene const & scene )
		: castor::OwnedBy< Scene const >{ scene }
		, m_device{ scene.getEngine()->getRenderSystem()->getRenderDevice() }
		, m_nodesData{ makeBuffer< ModelBufferConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getOwner()->getName() + cuT( "RenderNodesData" ) ) }
		, m_billboardsData{ makeBuffer< BillboardUboConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getOwner()->getName() + cuT( "BillboardsDimensions" ) ) }
	{
	}

	void SceneRenderNodes::clear()
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );

		for ( auto & nodes : m_submeshNodes )
		{
			for ( auto & node : nodes.second )
			{
				node.second->instance.setId( node.second->data, 0u );
			}
		}

		for ( auto & nodes : m_billboardNodes )
		{
			for ( auto & node : nodes.second )
			{
				node.second->instance.setId( 0u );
			}
		}

		m_submeshNodes.clear();
		m_billboardNodes.clear();
	}

	SubmeshRenderNode & SceneRenderNodes::createNode( Pass & pass
		, Submesh & data
		, Geometry & instance
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & pool = m_submeshNodes.emplace( pass.getTexturesMask().size(), DescriptorNodesPtrT< SubmeshRenderNode >{} ).first->second;
		auto it = pool.emplace( makeNodeHash( data, instance ), nullptr );

		if ( it.second )
		{
			it.first->second = castor::makeUnique< SubmeshRenderNode >( pass
				, data
				, instance );
			it.first->second->mesh = mesh;
			it.first->second->skeleton = skeleton;
			instance.setId( data, ++m_nodeId );
		}

		return *it.first->second;
	}

	BillboardRenderNode & SceneRenderNodes::createNode( Pass & pass
		, BillboardBase & instance )
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & pool = m_billboardNodes.emplace( pass.getTexturesMask().size(), DescriptorNodesPtrT< BillboardRenderNode >{} ).first->second;
		auto it = pool.emplace( makeNodeHash( instance ), nullptr );

		if ( it.second )
		{
			it.first->second = castor::makeUnique< BillboardRenderNode >( pass
				, instance );
			instance.setId( ++m_nodeId );
		}

		return *it.first->second;
	}

	void SceneRenderNodes::update( GpuUpdater & updater )
	{
		if ( auto nodesBuffer = m_nodesData->lock( 0u, ashes::WholeSize, 0u ) )
		{
			for ( auto & nodes : m_submeshNodes )
			{
				for ( auto & node : nodes.second )
				{
					doUpdateNode( *node.second, nodesBuffer );
				}
			}

			if ( auto billboardsBuffer = m_billboardsData->lock( 0u, ashes::WholeSize, 0u ) )
			{
				for ( auto & nodes : m_billboardNodes )
				{
					for ( auto & node : nodes.second )
					{
						doUpdateNode( *node.second, nodesBuffer, billboardsBuffer );
					}
				}

				m_billboardsData->flush( 0u, ashes::WholeSize );
				m_billboardsData->unlock();
			}

			m_nodesData->flush( 0u, ashes::WholeSize );
			m_nodesData->unlock();
		}
	}

	void SceneRenderNodes::doUpdateNode( SubmeshRenderNode & node
		, ModelBufferConfiguration * modelBufferData )
	{
		fillEntry( node.pass
			, *node.instance.getParent()
			, node.instance
			, modelBufferData[node.getId() - 1u] );
	}

	void SceneRenderNodes::doUpdateNode( BillboardRenderNode & node
		, ModelBufferConfiguration * modelBufferData
		, BillboardUboConfiguration * billboardBufferData )
	{
		fillEntry( node.pass
			, *node.instance.getNode()
			, node.instance
			, modelBufferData[node.getId() - 1u] );

		auto & billboardData = billboardBufferData[node.getId() - 1u];
		billboardData.dimensions = node.instance.getDimensions();
	}

	//*************************************************************************************************
}
