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
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Transform/VertexTransforming.hpp"
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

		static void rem( LightingModelID id
			, std::map< LightingModelID, size_t > & models )
		{
			auto it = models.find( id );

			if ( it != models.end()
				&& it->second > 0u )
			{
				--it->second;
			}
		}

		static void add( LightingModelID id
			, std::map< LightingModelID, size_t > & models )
		{
			auto it = models.emplace( id, 0u ).first;
			it->second++;
		}
	}

	//*********************************************************************************************

	RenderNodeType getRenderNodeType( ProgramFlags const & programFlags )
	{
		bool isFrontCulled = checkFlag( programFlags, ProgramFlag::eInvertNormals );

		if ( checkFlag( programFlags, ProgramFlag::eBillboards ) )
		{
			return isFrontCulled
				? RenderNodeType::eFrontBillboard
				: RenderNodeType::eBackBillboard;
		}

		if ( checkFlag( programFlags, ProgramFlag::eHasMesh ) )
		{
			return isFrontCulled
				? RenderNodeType::eFrontSubmeshMeshlet
				: RenderNodeType::eBackSubmeshMeshlet;
		}

		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return isFrontCulled
				? RenderNodeType::eFrontSubmeshInstanced
				: RenderNodeType::eBackSubmeshInstanced;
		}

		return isFrontCulled
			? RenderNodeType::eFrontSubmesh
			: RenderNodeType::eBackSubmesh;
	}

	//*********************************************************************************************

	SceneRenderNodes::SceneRenderNodes( Scene const & scene )
		: castor::OwnedBy< Scene const >{ scene }
		, m_device{ scene.getEngine()->getRenderSystem()->getRenderDevice() }
		, m_modelsData{ makeBuffer< ModelBufferConfiguration >( m_device
			, MaxObjectNodesCount
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getOwner()->getName() + cuT( "RenderNodesData" ) ) }
		, m_billboardsData{ makeBuffer< BillboardUboConfiguration >( m_device
			, MaxObjectNodesCount
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getOwner()->getName() + cuT( "BillboardsDimensions" ) ) }
		, m_modelsBuffer{ castor::makeArrayView( m_modelsData->lock( 0u, ashes::WholeSize, 0u )
			, m_modelsData->getCount() ) }
		, m_billboardsBuffer{ castor::makeArrayView( m_billboardsData->lock( 0u, ashes::WholeSize, 0u )
			, m_billboardsData->getCount() ) }
		, m_vertexTransform{ castor::makeUnique< VertexTransforming >( scene, m_device ) }
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

		for ( auto & node : m_submeshNodes )
		{
			node.second->instance.setId( *node.second->pass
				, node.second->data
				, nullptr
				, 0u );
		}

		for ( auto & node : m_billboardNodes )
		{
			node.second->instance.setId( *node.second->pass
				, nullptr
				, 0u );
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
		auto it = m_submeshNodes.emplace( scnrendnd::makeNodeHash( pass, data, instance ), nullptr );

		if ( it.second )
		{
			it.first->second = castor::makeUnique< SubmeshRenderNode >( pass
				, data
				, instance
				, m_modelsBuffer[m_nodeId] );
			auto & node = *it.first->second;
			node.mesh = mesh;
			node.skeleton = skeleton;
			m_nodesData.push_back( { &pass, instance.getParent(), &instance } );
			instance.setId( pass
				, data
				, &node
				, ++m_nodeId );
			instance.fillEntry( m_nodeId
				, pass
				, *instance.getParent()
				, data.getMeshletsCount()
				, node.modelData );
			scnrendnd::add( pass.getLightingModelId(), m_lightingModels );

			if ( data.isDynamic() )
			{
				static GpuBufferOffsetT< castor::Point4f > const morphTargets{};
				static GpuBufferOffsetT< MorphingWeightsConfiguration > const morphingWeights{};
				static GpuBufferOffsetT< SkinningTransformsConfiguration > const skinTransforms{};

				if ( data.hasComponent( MorphComponent::Name )
					&& data.hasComponent( SkinComponent::Name ) )
				{
					m_vertexTransform->registerNode( node
						, data.getMorphTargets()
						, getOwner()->getAnimatedObjectGroupCache().getMorphingWeights()
						, getOwner()->getAnimatedObjectGroupCache().getSkinningTransformsBuffer() );
				}
				else if ( data.hasComponent( MorphComponent::Name ) )
				{
					m_vertexTransform->registerNode( node
						, data.getMorphTargets()
						, getOwner()->getAnimatedObjectGroupCache().getMorphingWeights()
						, skinTransforms );
				}
				else
				{
					m_vertexTransform->registerNode( node
						, morphTargets
						, morphingWeights
						, getOwner()->getAnimatedObjectGroupCache().getSkinningTransformsBuffer() );
				}
			}

			m_dirty = true;
		}

		return *it.first->second;
	}

	BillboardRenderNode & SceneRenderNodes::createNode( Pass & pass
		, BillboardBase & instance )
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto it = m_billboardNodes.emplace( scnrendnd::makeNodeHash( pass, instance ), nullptr );

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
			instance.fillEntry( m_nodeId
				, pass
				, *instance.getNode()
				, 0u
				, it.first->second->modelData );
			scnrendnd::add( pass.getLightingModelId(), m_lightingModels );
			m_dirty = true;
		}

		return *it.first->second;
	}

	SubmeshRenderNode const * SceneRenderNodes::getSubmeshNode( uint32_t nodeId )
	{
		for ( auto & nodeIt : m_submeshNodes )
		{
			if ( nodeIt.second->getId() == nodeId )
			{
				return nodeIt.second.get();
			}
		}

		return nullptr;
	}

	BillboardRenderNode const * SceneRenderNodes::getBillboardNode( uint32_t nodeId )
	{
		for ( auto & nodeIt : m_billboardNodes )
		{
			if ( nodeIt.second->getId() == nodeId )
			{
				return nodeIt.second.get();
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

		for ( auto & pass : oldMaterial )
		{
			auto submeshIt = m_submeshNodes.find( scnrendnd::makeNodeHash( *pass, data, instance ) );

			if ( submeshIt != m_submeshNodes.end() )
			{
				scnrendnd::rem( pass->getLightingModelId(), m_lightingModels );
				auto node = std::move( submeshIt->second );
				m_submeshNodes.erase( submeshIt );

				for ( auto & culler : m_cullers )
				{
					culler->removeCulled( *node );
				}

				nodes.emplace_back( instance.getId( *pass, data ), std::move( node ) );
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
				scnrendnd::add( pass->getLightingModelId(), m_lightingModels );
				auto submeshIt = m_submeshNodes.emplace( scnrendnd::makeNodeHash( *pass, data, instance ), nullptr ).first;
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
				auto animMesh = data.hasMorphComponent()
					? static_cast< AnimatedMesh * >( findAnimatedObject( *getOwner(), instance.getName() + cuT( "_Mesh" ) ) )
					: nullptr;
				auto animSkeleton = data.hasSkinComponent()
					? static_cast< AnimatedSkeleton * >( findAnimatedObject( *getOwner(), instance.getName() + cuT( "_Skeleton" ) ) )
					: nullptr;

				while ( passIt != newMaterial.end() )
				{
					auto pass = passIt->get();
					scnrendnd::add( pass->getLightingModelId(), m_lightingModels );
					createNode( *pass
						, data
						, instance
						, animMesh
						, animSkeleton );
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

		for ( auto & pass : oldMaterial )
		{
			auto billboardIt = m_billboardNodes.find( scnrendnd::makeNodeHash( *pass, billboard ) );

			if ( billboardIt != m_billboardNodes.end() )
			{
				scnrendnd::rem( pass->getLightingModelId(), m_lightingModels );
				auto node = std::move( billboardIt->second );
				m_billboardNodes.erase( billboardIt );

				for ( auto & culler : m_cullers )
				{
					culler->removeCulled( *node );
				}

				nodes.emplace_back( billboard.getId( *pass ), std::move( node ) );
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
				scnrendnd::add( pass->getLightingModelId(), m_lightingModels );
				auto billboardIt = m_billboardNodes.emplace( scnrendnd::makeNodeHash( *pass, billboard ), nullptr ).first;
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
				scnrendnd::add( pass->getLightingModelId(), m_lightingModels );
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
		
		for ( auto & nodeIt : m_submeshNodes )
		{
			auto & node = nodeIt.second;
			auto & instantiation = node->data.getInstantiation();
			node->mesh = node->data.hasMorphComponent()
				? static_cast< AnimatedMesh * >( findAnimatedObject( *getOwner(), node->instance.getName() + cuT( "_Mesh" ) ) )
				: nullptr;
			node->skeleton = node->data.hasSkinComponent()
				? static_cast< AnimatedSkeleton * >( findAnimatedObject( *getOwner(), node->instance.getName() + cuT( "_Skeleton" ) ) )
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
					buffer.objectIDs.nodeId = node->getId();

					if ( node->mesh )
					{
						CU_Require( node->mesh->getId( node->data ) > 0u );
						buffer.objectIDs.morphingId = node->mesh->getId( node->data ) - 1u;
					}

					if ( node->skeleton )
					{
						CU_Require( node->skeleton->getId() > 0u );
						buffer.objectIDs.skinningId = node->skeleton->getId() - 1u;
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

	bool SceneRenderNodes::hasNodes( LightingModelID lightingModelId )const
	{
		auto it = m_lightingModels.find( lightingModelId );
		return it == m_lightingModels.end()
			? false
			: it->second > 0u;
	}

	crg::FramePass const & SceneRenderNodes::createVertexTransformPass( crg::FramePassGroup & graph )
	{
		return m_vertexTransform->createPass( graph );
	}

	//*************************************************************************************************
}
