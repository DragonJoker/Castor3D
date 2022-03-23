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

	namespace scnrendnd
	{
		static size_t makeNodeHash( Pass & pass
			, Submesh & data
			, Geometry & instance )
		{
			auto hash = std::hash< Submesh * >{}( &data );
			hash = castor::hashCombinePtr( hash, instance );
			hash = castor::hashCombinePtr( hash, pass );
			return hash;
		}

		static size_t makeNodeHash( Pass & pass
			, BillboardBase & instance )
		{
			auto hash = std::hash< BillboardBase * >{}( &instance );
			hash = castor::hashCombinePtr( hash, pass );
			return hash;
		}

		static void fillEntry( Pass const & pass
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
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getOwner()->getName() + cuT( "RenderNodesData" ) ) }
		, m_billboardsData{ makeBuffer< BillboardUboConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
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
				node.second->instance.setId( node.second->pass, node.second->data, 0u );
			}
		}

		for ( auto & nodes : m_billboardNodes )
		{
			for ( auto & node : nodes.second )
			{
				node.second->instance.setId( node.second->pass, 0u );
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
		auto it = pool.emplace( scnrendnd::makeNodeHash( pass, data, instance ), nullptr );

		if ( it.second )
		{
			it.first->second = castor::makeUnique< SubmeshRenderNode >( pass
				, data
				, instance );
			it.first->second->mesh = mesh;
			it.first->second->skeleton = skeleton;
			instance.setId( pass, data, ++m_nodeId );
			m_dirty = true;
		}

		return *it.first->second;
	}

	BillboardRenderNode & SceneRenderNodes::createNode( Pass & pass
		, BillboardBase & instance )
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & pool = m_billboardNodes.emplace( pass.getTexturesMask().size(), DescriptorNodesPtrT< BillboardRenderNode >{} ).first->second;
		auto it = pool.emplace( scnrendnd::makeNodeHash( pass, instance ), nullptr );

		if ( it.second )
		{
			it.first->second = castor::makeUnique< BillboardRenderNode >( pass
				, instance );
			instance.setId( pass, ++m_nodeId );
			m_dirty = true;
		}

		return *it.first->second;
	}

	SubmeshRenderNode const * SceneRenderNodes::getSubmeshNode( uint32_t nodeId )
	{
		for ( auto & poolsIt : m_submeshNodes )
		{
			for ( auto & nodeIt : poolsIt.second )
			{
				if ( nodeIt.second->getId() == nodeId )
				{
					return nodeIt.second.get();
				}
			}
		}

		return nullptr;
	}

	BillboardRenderNode const * SceneRenderNodes::getBillboardNode( uint32_t nodeId )
	{
		for ( auto & poolsIt : m_billboardNodes )
		{
			for ( auto & nodeIt : poolsIt.second )
			{
				if ( nodeIt.second->getId() == nodeId )
				{
					return nodeIt.second.get();
				}
			}
		}

		return nullptr;
	}

	void SceneRenderNodes::update( CpuUpdater & updater )
	{
		if ( !m_dirty )
		{
			return;
		}

		std::map< Submesh const *, std::map< Pass const *, uint32_t > > indices;

		for ( auto & nodes : m_submeshNodes )
		{
			for ( auto & nodeIt : nodes.second )
			{
				auto & node = nodeIt.second;
				auto & instantiation = node->data.getInstantiation();

				if ( instantiation.isInstanced() )
				{
					auto & passes = indices.emplace( &node->data, std::map< Pass const *, uint32_t >{} ).first->second;
					auto & index = passes.emplace( &node->pass, 0u ).first->second;
					auto it = instantiation.find( node->pass.getOwner() );
					CU_Require( it != instantiation.end() );
					CU_Require( node->getId() > 0u );

					if ( it->second.data.size() > index )
					{
						auto & buffer = it->second.data[index++];
						buffer.m_objectIDs->x = node->getId();

						if ( node->mesh )
						{
							CU_Require( node->mesh->getId() > 0u );
							buffer.m_objectIDs->y = node->mesh->getId() - 1u;
						}

						if ( node->skeleton )
						{
							CU_Require( node->skeleton->getId() > 0u );
							buffer.m_objectIDs->z = node->skeleton->getId() - 1u;
						}
					}
				}
			}
		}

		m_dirty = false;
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
		scnrendnd::fillEntry( node.pass
			, *node.instance.getParent()
			, node.instance
			, modelBufferData[node.getId() - 1u] );
	}

	void SceneRenderNodes::doUpdateNode( BillboardRenderNode & node
		, ModelBufferConfiguration * modelBufferData
		, BillboardUboConfiguration * billboardBufferData )
	{
		scnrendnd::fillEntry( node.pass
			, *node.instance.getNode()
			, node.instance
			, modelBufferData[node.getId() - 1u] );

		auto & billboardData = billboardBufferData[node.getId() - 1u];
		billboardData.dimensions = node.instance.getDimensions();
	}

	//*************************************************************************************************
}
