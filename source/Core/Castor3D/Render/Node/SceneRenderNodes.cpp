#include "Castor3D/Render/Node/SceneRenderNodes.hpp"

#include "Castor3D/DebugDefines.hpp"
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
#include "Castor3D/Render/Culling/SceneCuller.hpp"
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
		static size_t makeNodeHash( Pass const & pass
			, Submesh const & data
			, Geometry const & instance )
		{
			auto hash = std::hash< Submesh const * >{}( &data );
			hash = castor::hashCombinePtr( hash, instance );
			hash = castor::hashCombinePtr( hash, pass );
			return hash;
		}

		static size_t makeNodeHash( Pass const & pass
			, BillboardBase const & instance )
		{
			auto hash = std::hash< BillboardBase const * >{}( &instance );
			hash = castor::hashCombinePtr( hash, pass );
			return hash;
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
		, m_modelsData{ makeBuffer< ModelBufferConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getOwner()->getName() + cuT( "RenderNodesData" ) ) }
		, m_billboardsData{ makeBuffer< BillboardUboConfiguration >( m_device
			, 250'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getOwner()->getName() + cuT( "BillboardsDimensions" ) ) }
		, m_modelsBuffer{ m_modelsData->lock( 0u, ashes::WholeSize, 0u ) }
		, m_billboardsBuffer{ m_billboardsData->lock( 0u, ashes::WholeSize, 0u ) }
	{
#if C3D_DebugTimers
		m_timerRenderNodes = castor::makeUnique< crg::FramePassTimer >( m_device.makeContext(), getOwner()->getName() + "/RenderNodes" );
		getOwner()->getEngine()->registerTimer( getOwner()->getName() + "/RenderNodes", *m_timerRenderNodes );
#endif
	}

	SceneRenderNodes::~SceneRenderNodes()
	{
#if C3D_DebugTimers
		getOwner()->getEngine()->unregisterTimer( getOwner()->getName() + "/RenderNodes", *m_timerRenderNodes );
		m_timerRenderNodes.reset();
#endif
	}

	void SceneRenderNodes::registerCuller( SceneCuller & culler )
	{
		m_cullers.emplace_back( &culler );
	}

	void SceneRenderNodes::unregisterCuller( SceneCuller & culler )
	{
		auto it = std::find( m_cullers.begin()
			, m_cullers.end()
			, &culler );
		CU_Require( it != m_cullers.end() );
		m_cullers.erase( it );
	}

	void SceneRenderNodes::clear()
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );

		for ( auto & nodes : m_submeshNodes )
		{
			for ( auto & node : nodes.second )
			{
				node.second->instance.setId( *node.second->pass
					, node.second->data
					, nullptr
					, 0u );
			}
		}

		for ( auto & nodes : m_billboardNodes )
		{
			for ( auto & node : nodes.second )
			{
				node.second->instance.setId( *node.second->pass
					, nullptr
					, 0u );
			}
		}

		m_nodesData.clear();
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
		auto & pool = m_submeshNodes.emplace( pass.getTexturesMask().size(), NodesPtrMapT< SubmeshRenderNode >{} ).first->second;
		auto it = pool.emplace( scnrendnd::makeNodeHash( pass, data, instance ), nullptr );

		if ( it.second )
		{
			it.first->second = castor::makeUnique< SubmeshRenderNode >( pass
				, data
				, instance
				, m_modelsBuffer[m_nodeId] );
			it.first->second->mesh = mesh;
			it.first->second->skeleton = skeleton;
			m_nodesData.push_back( { &pass, instance.getParent(), &instance } );
			instance.setId( pass
				, data
				, it.first->second.get()
				, ++m_nodeId );
			instance.fillEntry( pass
				, *instance.getParent()
				, it.first->second->modelData );
			m_dirty = true;
		}

		return *it.first->second;
	}

	BillboardRenderNode & SceneRenderNodes::createNode( Pass & pass
		, BillboardBase & instance )
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & pool = m_billboardNodes.emplace( pass.getTexturesMask().size(), NodesPtrMapT< BillboardRenderNode >{} ).first->second;
		auto it = pool.emplace( scnrendnd::makeNodeHash( pass, instance ), nullptr );

		if ( it.second )
		{
			it.first->second = castor::makeUnique< BillboardRenderNode >( pass
				, instance
				, m_modelsBuffer[m_nodeId]
				, m_billboardsBuffer[m_nodeId] );
			m_nodesData.push_back( { &pass, instance.getNode(), &instance } );
			instance.setId( pass
				, it.first->second.get()
				, ++m_nodeId );
			instance.fillEntry( pass
				, *instance.getNode()
				, it.first->second->modelData );
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

	void SceneRenderNodes::reportPassChange( Submesh & data
		, Geometry & instance
		, Material const & oldMaterial
		, Material const & newMaterial )
	{
		std::vector< std::pair< uint32_t, SubmeshRenderNodeUPtr > > nodes;

		for ( auto pass : oldMaterial )
		{
			auto poolIt = m_submeshNodes.find( pass->getTexturesMask().size() );

			if ( poolIt != m_submeshNodes.end() )
			{
				auto submeshIt = poolIt->second.find( scnrendnd::makeNodeHash( *pass, data, instance ) );

				if ( submeshIt != poolIt->second.end() )
				{
					auto node = std::move( submeshIt->second );
					poolIt->second.erase( submeshIt );

					if ( poolIt->second.empty() )
					{
						m_submeshNodes.erase( poolIt );
					}

					for ( auto & culler : m_cullers )
					{
						culler->removeCulled( *node );
					}

					nodes.emplace_back( instance.getId( *pass, data ), std::move( node ) );
				}
			}
		}

		if ( !nodes.empty() )
		{
			auto nodeIt = nodes.begin();
			auto passIt = newMaterial.begin();

			while ( nodeIt != nodes.end()
				&& passIt != newMaterial.end() )
			{
				auto pass = passIt->get();
				auto poolIt = m_submeshNodes.emplace( pass->getTexturesMask().size(), NodesPtrMapT< SubmeshRenderNode >{} ).first;
				auto submeshIt = poolIt->second.emplace( scnrendnd::makeNodeHash( *pass, data, instance ), nullptr ).first;
				submeshIt->second = std::move( nodeIt->second );

				auto it = std::find_if( m_nodesData.begin()
					, m_nodesData.end()
					, [submeshIt]( NodeData const & lookup )
					{
						return lookup.object == &submeshIt->second->instance
							&& lookup.pass == submeshIt->second->pass;
					} );
				CU_Require( it != m_nodesData.end() );
				it->pass = pass;
				submeshIt->second->pass = pass;
				instance.setId( *pass
					, data
					, submeshIt->second.get()
					, nodeIt->first );

				++nodeIt;
				++passIt;
			}

			if ( passIt != newMaterial.end() )
			{
				auto submeshFlags = data.getProgramFlags( newMaterial );
				auto animMesh = checkFlag( submeshFlags, ProgramFlag::eMorphing )
					? std::static_pointer_cast< AnimatedMesh >( findAnimatedObject( *getOwner(), instance.getName() + cuT( "_Mesh" ) ) )
					: nullptr;
				auto animSkeleton = checkFlag( submeshFlags, ProgramFlag::eSkinning )
					? std::static_pointer_cast< AnimatedSkeleton >( findAnimatedObject( *getOwner(), instance.getName() + cuT( "_Skeleton" ) ) )
					: nullptr;

				while ( passIt != newMaterial.end() )
				{
					auto pass = passIt->get();
					createNode( *pass
						, data
						, instance
						, animMesh.get()
						, animSkeleton.get() );
					++passIt;
				}
			}
		}
	}

	void SceneRenderNodes::reportPassChange( BillboardBase & billboard
		, Material const & oldMaterial
		, Material const & newMaterial )
	{
		std::vector< std::pair< uint32_t, BillboardRenderNodeUPtr > > nodes;

		for ( auto pass : oldMaterial )
		{
			auto poolIt = m_billboardNodes.find( pass->getTexturesMask().size() );

			if ( poolIt != m_billboardNodes.end() )
			{
				auto billboardIt = poolIt->second.find( scnrendnd::makeNodeHash( *pass, billboard ) );

				if ( billboardIt != poolIt->second.end() )
				{
					auto node = std::move( billboardIt->second );
					poolIt->second.erase( billboardIt );

					if ( poolIt->second.empty() )
					{
						m_billboardNodes.erase( poolIt );
					}

					for ( auto & culler : m_cullers )
					{
						culler->removeCulled( *node );
					}

					nodes.emplace_back( billboard.getId( *pass ), std::move( node ) );
				}
			}
		}

		if ( !nodes.empty() )
		{
			auto nodeIt = nodes.begin();
			auto passIt = newMaterial.begin();

			while ( nodeIt != nodes.end()
				&& passIt != newMaterial.end() )
			{
				auto pass = passIt->get();
				auto poolIt = m_billboardNodes.emplace( pass->getTexturesMask().size(), NodesPtrMapT< BillboardRenderNode >{} ).first;
				auto billboardIt = poolIt->second.emplace( scnrendnd::makeNodeHash( *pass, billboard ), nullptr ).first;
				billboardIt->second = std::move( nodeIt->second );

				auto it = std::find_if( m_nodesData.begin()
					, m_nodesData.end()
					, [billboardIt]( NodeData const & lookup )
					{
						return lookup.object == &billboardIt->second->instance
							&& lookup.pass == billboardIt->second->pass;
					} );
				CU_Require( it != m_nodesData.end() );
				it->pass = pass;
				billboardIt->second->pass = pass;
				billboard.setId( *pass
					, billboardIt->second.get()
					, nodeIt->first );

				++nodeIt;
				++passIt;
			}

			while ( passIt != newMaterial.end() )
			{
				auto pass = passIt->get();
				createNode( *pass
					, billboard );
				++passIt;
			}
		}
	}

	void SceneRenderNodes::update( CpuUpdater & updater )
	{
		if ( !m_dirty && updater.dirtyScenes[getOwner()].dirtyNodes.empty() )
		{
			return;
		}

#if C3D_DebugTimers
		auto block( m_timerRenderNodes->start() );
#endif
		std::map< Submesh const *, std::map< Pass const *, uint32_t > > indices;

		for ( auto & nodes : m_submeshNodes )
		{
			for ( auto & nodeIt : nodes.second )
			{
				auto & node = nodeIt.second;
				auto & instantiation = node->data.getInstantiation();
				auto submeshFlags = node->data.getProgramFlags( *node->pass->getOwner() );
				node->mesh = checkFlag( submeshFlags, ProgramFlag::eMorphing )
					? std::static_pointer_cast< AnimatedMesh >( findAnimatedObject( *getOwner(), node->instance.getName() + cuT( "_Mesh" ) ) ).get()
					: nullptr;
				node->skeleton = checkFlag( submeshFlags, ProgramFlag::eSkinning )
					? std::static_pointer_cast< AnimatedSkeleton >( findAnimatedObject( *getOwner(), node->instance.getName() + cuT( "_Skeleton" ) ) ).get()
					: nullptr;

				if ( instantiation.isInstanced()
					&& node->instance.getParent()->isVisible() )
				{
					auto & passes = indices.emplace( &node->data, std::map< Pass const *, uint32_t >{} ).first->second;
					auto & index = passes.emplace( node->pass, 0u ).first->second;
					auto it = instantiation.find( *node->pass->getOwner() );
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
		if ( m_nodesData.empty() )
		{
			return;
		}

		m_modelsData->flush( 0u, m_nodesData.size() );

		if ( !m_billboardNodes.empty() )
		{
			m_billboardsData->flush( 0u, m_nodesData.size() );
		}
	}

	//*************************************************************************************************
}
