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
		size_t makeLayoutHash( size_t texturesCount
			, BillboardBase const * billboard
			, Submesh const * submesh
			, AnimatedMesh const * mesh
			, AnimatedSkeleton const * skeleton )
		{
			size_t result{};
			result += texturesCount;
			return result;
		}

		size_t makeNodeHash( SceneNode & sceneNode
			, Submesh & data
			, Geometry & instance )
		{
			auto hash = std::hash< SceneNode * >{}( &sceneNode );
			hash = castor::hashCombinePtr( hash, data );
			hash = castor::hashCombinePtr( hash, instance );
			return hash;
		}

		size_t makeNodeHash( SceneNode & sceneNode
			, BillboardBase & instance )
		{
			auto hash = std::hash< SceneNode * >{}( &sceneNode );
			hash = castor::hashCombinePtr( hash, instance );
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

		//*****************************************************************************************

		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( size_t texturesCount )
		{
			ashes::VkDescriptorSetLayoutBindingArray texBindings;

			if ( texturesCount )
			{
				texBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT
					, uint32_t( texturesCount ) ) );
			}

			return texBindings;
		}

		//*****************************************************************************************

		castor::String getName( BillboardRenderNode const & node )
		{
			return node.sceneNode.getName() + "Billboard";
		}

		castor::String getName( SubmeshRenderNode const & node )
		{
			return node.instance.getName();
		}

		template< typename NodeT >
		void doInitialiseTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, ashes::DescriptorPool const & descriptorPool
			, NodeT & node )
		{
			if ( node.texDescriptorSet )
			{
				auto & descriptorSet = *node.texDescriptorSet;
				ashes::WriteDescriptorSetArray writes;
				auto textures = node.passNode.pass.getTexturesMask();

				if ( !textures.empty() )
				{
					uint32_t index = 0u;
					node.passNode.fillDescriptor( descriptorSet.getLayout()
						, index
						, writes
						, textures );
				}

				descriptorSet.setBindings( std::move( writes ) );
				descriptorSet.update();
			}
		}

		template< typename NodeT >
		void doInitialiseNode( Engine & engine
			, SceneRenderNodes::DescriptorSetPools const & pools
			, SceneRenderNodes::DescriptorSetLayouts const & descriptorLayout
			, NodeT & node )
		{
			if ( !descriptorLayout.tex->getBindings().empty() )
			{
				doInitialiseTextureDescriptor( *descriptorLayout.tex
					, *pools.tex
					, node );
			}
		}

		template< typename NodeT >
		SceneRenderNodes::DescriptorNodesT< NodeT > & getNodesPool( size_t texturesCount
			, bool instanced
			, BillboardBase const * billboard
			, Submesh const * submesh
			, AnimatedMesh const * mesh
			, AnimatedSkeleton const * skeleton
			, Engine & engine
			, ashes::DescriptorSetLayout * texLayout
			, ashes::DescriptorPool * texPool
			, ashes::DescriptorSet * texSet
			, SceneRenderNodes::DescriptorNodesPoolsT< NodeT > & nodesPools )
		{
			auto iresPool = nodesPools.emplace( makeLayoutHash( texturesCount
				, billboard
				, submesh
				, mesh
				, skeleton ), nullptr );
			auto itPool = iresPool.first;

			if ( iresPool.second )
			{
				itPool->second = std::make_unique< SceneRenderNodes::DescriptorNodesT< NodeT > >( engine
					, texturesCount
					, texLayout
					, texPool
					, texSet
					, instanced
					, billboard
					, submesh
					, mesh
					, skeleton );
			}

			return *itPool->second;
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

	static constexpr uint32_t MaxPoolSize = 50u;

	SceneRenderNodes::DescriptorSetPools::DescriptorSetPools( RenderDevice const & device
		, SceneRenderNodes::DescriptorCounts const & counts
		, ashes::DescriptorPool * texPool )
	{
		ashes::VkDescriptorPoolSizeArray bufSizes;

		if ( texPool )
		{
			tex = texPool;
		}
		else
		{
			ashes::VkDescriptorPoolSizeArray texSizes;

			if ( counts.combinedImages )
			{
				texSizes.push_back( { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, counts.combinedImages } );
			}

			if ( !texSizes.empty() )
			{
				ownTex = device->createDescriptorPool( VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
					, MaxPoolSize
					, std::move( texSizes ) );
				tex = ownTex.get();
			}
		}
	}

	//*********************************************************************************************

	SceneRenderNodes::DescriptorCounts::DescriptorCounts( size_t textureCount
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		update( textureCount
			, billboard
			, submesh
			, mesh
			, skeleton );
	}

	void SceneRenderNodes::DescriptorCounts::update( size_t textureCount
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		combinedImages += uint32_t( textureCount );
	}
	
	//*********************************************************************************************

	SceneRenderNodes::DescriptorPools::DescriptorPools( Engine & engine
		, DescriptorSetLayouts * layouts
		, DescriptorCounts * counts
		, ashes::DescriptorPool * texPool
		, ashes::DescriptorSet * texSet )
		: m_engine{ engine }
		, m_layouts{ layouts }
		, m_counts{ counts }
		, m_texPool{ texPool }
		, m_texSet{ texSet }
	{
	}

	void SceneRenderNodes::DescriptorPools::allocate( SubmeshRenderNode & node )
	{
		auto ires = m_allocated.emplace( intptr_t( &node ), NodeSet{} );
		auto it = ires.first;

		if ( !ires.second )
		{
			return;
		}

		if ( !m_available )
		{
			m_pools.emplace_back( m_engine.getRenderSystem()->getRenderDevice()
				, *m_counts
				, m_texPool );
			m_available += MaxPoolSize;
		}

		--m_available;
		doAllocateTex( m_pools.back(), node, it->second );
	}
	
	void SceneRenderNodes::DescriptorPools::allocate( BillboardRenderNode & node )
	{
		auto ires = m_allocated.emplace( intptr_t( &node ), NodeSet{} );
		auto it = ires.first;

		if ( !ires.second )
		{
			return;
		}

		if ( !m_available )
		{
			m_pools.emplace_back( m_engine.getRenderSystem()->getRenderDevice()
				, *m_counts
				, m_texPool );
		}

		--m_available;
		doAllocateTex( m_pools.back(), node, it->second );
	}

	void SceneRenderNodes::DescriptorPools::deallocate( SubmeshRenderNode & node )
	{
		auto it = m_allocated.find( intptr_t( &node ) );

		if ( it != m_allocated.end() )
		{
			m_allocated.erase( it );
			node.texDescriptorSet = nullptr;
		}
	}

	void SceneRenderNodes::DescriptorPools::deallocate( BillboardRenderNode & node )
	{
		auto it = m_allocated.find( intptr_t( &node ) );

		if ( it != m_allocated.end() )
		{
			m_allocated.erase( it );
			node.texDescriptorSet = nullptr;
		}
	}

	void SceneRenderNodes::DescriptorPools::doAllocateTex( DescriptorSetPools const & pools
		, SubmeshRenderNode & node
		, NodeSet & allocated )
	{
		if ( m_texSet )
		{
			allocated.texSet = m_texSet;
			node.texDescriptorSet = nullptr;
		}
		else if ( !m_layouts->tex->getBindings().empty() )
		{
			allocated.texPool = &( *pools.tex );
			allocated.ownTexSet = pools.tex->createDescriptorSet( getName( node ) + "Tex"
				, *m_layouts->tex
				, RenderPipeline::eTextures );
			allocated.texSet = allocated.ownTexSet.get();
			node.texDescriptorSet = allocated.texSet;
			doInitialiseTextureDescriptor( *m_layouts->tex
				, *pools.tex
				, node );
		}
	}

	void SceneRenderNodes::DescriptorPools::doAllocateTex( DescriptorSetPools const & pools
		, BillboardRenderNode & node
		, NodeSet & allocated )
	{
		if ( m_texSet )
		{
			allocated.texSet = m_texSet;
			node.texDescriptorSet = nullptr;
		}
		else if ( !m_layouts->tex->getBindings().empty() )
		{
			allocated.texPool = &( *pools.tex );
			allocated.ownTexSet = pools.tex->createDescriptorSet( getName( node ) + "Tex"
				, *m_layouts->tex
				, RenderPipeline::eTextures );
			allocated.texSet = allocated.ownTexSet.get();
			node.texDescriptorSet = allocated.texSet;
			doInitialiseTextureDescriptor( *m_layouts->tex
				, *pools.tex
				, node );
		}
	}

	//*********************************************************************************************

	SceneRenderNodes::DescriptorSetLayouts::DescriptorSetLayouts( Engine & engine
		, size_t texturesCount
		, bool instanced
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton
		, ashes::DescriptorSetLayout * texLayout )
		: tex{ texLayout }
	{
		if ( !tex )
		{
			auto & device = engine.getRenderSystem()->getRenderDevice();
			ownTex = device->createDescriptorSetLayout( doCreateTextureBindings( texturesCount ) );
			tex = ownTex.get();
		}
	}

	//*********************************************************************************************

	template<>
	SceneRenderNodes::DescriptorNodesT< SubmeshRenderNode  >::DescriptorNodesT( Engine & engine
		, size_t texturesCount
		, ashes::DescriptorSetLayout * texLayout
		, ashes::DescriptorPool * texPool
		, ashes::DescriptorSet * texSet
		, bool instanced
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
		: counts{ texturesCount
			, billboard
			, submesh
			, mesh
			, skeleton }
		, layouts{ engine
			, texturesCount
			, instanced
			, billboard
			, submesh
			, mesh
			, skeleton
			, texLayout }
		, pools{ engine, &layouts, &counts, texPool, texSet }
	{
	}
	
	template<>
	SceneRenderNodes::DescriptorNodesT< BillboardRenderNode  >::DescriptorNodesT( Engine & engine
		, size_t texturesCount
		, ashes::DescriptorSetLayout * texLayout
		, ashes::DescriptorPool * texPool
		, ashes::DescriptorSet * texSet
		, bool instanced
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
		: counts{ texturesCount
			, billboard
			, submesh
			, mesh
			, skeleton }
		, layouts{ engine
			, texturesCount
			, instanced
			, billboard
			, submesh
			, mesh
			, skeleton
			, texLayout }
		, pools{ engine, &layouts, &counts, texPool, texSet }
	{
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

	void SceneRenderNodes::initialiseNodes( RenderDevice const & device )
	{
		for ( auto & nodes : m_submeshNodes )
		{
			for ( auto & node : nodes.second->nodes )
			{
				nodes.second->pools.allocate( *node.second );
			}
		}

		for ( auto & nodes : m_billboardNodes )
		{
			for ( auto & node : nodes.second->nodes )
			{
				nodes.second->pools.allocate( *node.second );
			}
		}
	}

	void SceneRenderNodes::clear()
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );

		for ( auto & nodes : m_submeshNodes )
		{
			for ( auto & node : nodes.second->nodes )
			{
				node.second->instance.setId( node.second->data, 0u );
			}
		}

		for ( auto & nodes : m_billboardNodes )
		{
			for ( auto & node : nodes.second->nodes )
			{
				node.second->instance.setId( 0u );
			}
		}

		m_submeshNodes.clear();
		m_billboardNodes.clear();
	}

	SubmeshRenderNode & SceneRenderNodes::createNode( PassRenderNode passNode
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		auto & pass = passNode.pass;
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & engine = *getOwner()->getEngine();
		auto & pool = getNodesPool( pass.getTexturesMask().size()
			, data.getInstantiation().isInstanced( pass.getOwner() )
			, nullptr
			, &data
			, mesh
			, skeleton
			, engine
			, getOwner()->getBindlessTexDescriptorLayout()
			, getOwner()->getBindlessTexDescriptorPool()
			, getOwner()->getBindlessTexDescriptorSet()
			, m_submeshNodes );
		auto it = pool.nodes.emplace( makeNodeHash( sceneNode, data, instance ), nullptr );

		if ( it.second )
		{
			doUpdateDescriptorsCounts( passNode.pass, nullptr, &data, mesh, skeleton );
			it.first->second = castor::makeUnique< SubmeshRenderNode >( std::move( passNode )
				, buffers
				, sceneNode
				, data
				, instance );
			it.first->second->mesh = mesh;
			it.first->second->skeleton = skeleton;
			instance.setId( data, ++m_nodeId );
		}

		return *it.first->second;
	}

	BillboardRenderNode & SceneRenderNodes::createNode( PassRenderNode passNode
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & instance )
	{
		auto & pass = passNode.pass;
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & engine = *getOwner()->getEngine();
		auto & pool = getNodesPool( pass.getTexturesMask().size()
			, false
			, &instance
			, nullptr
			, nullptr
			, nullptr
			, engine
			, getOwner()->getBindlessTexDescriptorLayout()
			, getOwner()->getBindlessTexDescriptorPool()
			, getOwner()->getBindlessTexDescriptorSet()
			, m_billboardNodes );
		auto it = pool.nodes.emplace( makeNodeHash( sceneNode, instance ), nullptr );

		if ( it.second )
		{
			doUpdateDescriptorsCounts( passNode.pass, &instance, nullptr, nullptr, nullptr );
			it.first->second = castor::makeUnique< BillboardRenderNode >( std::move( passNode )
				, buffers
				, sceneNode
				, instance );
			instance.setId( ++m_nodeId );
		}

		return *it.first->second;
	}

	ashes::DescriptorSetLayoutCRefArray SceneRenderNodes::getDescriptorSetLayouts( Pass const & pass
		, BillboardBase const & billboard )
	{
		return doGetDescriptorSetLayouts( pass, &billboard, nullptr, nullptr, nullptr );
	}

	ashes::DescriptorSetLayoutCRefArray SceneRenderNodes::getDescriptorSetLayouts( Pass const & pass
		, Submesh const & submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		return doGetDescriptorSetLayouts( pass, nullptr, &submesh, mesh, skeleton );
	}

	void SceneRenderNodes::update( GpuUpdater & updater )
	{
		if ( auto nodesBuffer = m_nodesData->lock( 0u, ashes::WholeSize, 0u ) )
		{
			for ( auto & nodes : m_submeshNodes )
			{
				for ( auto & node : nodes.second->nodes )
				{
					doUpdateNode( *node.second, nodesBuffer );
				}
			}

			if ( auto billboardsBuffer = m_billboardsData->lock( 0u, ashes::WholeSize, 0u ) )
			{
				for ( auto & nodes : m_billboardNodes )
				{
					for ( auto & node : nodes.second->nodes )
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

	ashes::DescriptorSetLayoutCRefArray SceneRenderNodes::doGetDescriptorSetLayouts( Pass const & pass
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & engine = *getOwner()->getEngine();
		DescriptorSetLayouts * layouts;

		if ( billboard )
		{
			auto & pool = getNodesPool( pass.getTexturesMask().size()
				, false
				, billboard
				, submesh
				, mesh
				, skeleton
				, engine
				, getOwner()->getBindlessTexDescriptorLayout()
				, getOwner()->getBindlessTexDescriptorPool()
				, getOwner()->getBindlessTexDescriptorSet()
				, m_billboardNodes );
			layouts = &pool.layouts;
		}
		else
		{
			auto & pool = getNodesPool( pass.getTexturesMask().size()
				, submesh->getInstantiation().isInstanced( pass.getOwner() )
				, billboard
				, submesh
				, mesh
				, skeleton
				, engine
				, getOwner()->getBindlessTexDescriptorLayout()
				, getOwner()->getBindlessTexDescriptorPool()
				, getOwner()->getBindlessTexDescriptorSet()
				, m_submeshNodes );
			layouts = &pool.layouts;
		}

		ashes::DescriptorSetLayoutCRefArray result;
		result.emplace_back( *layouts->tex );
		return result;
	}

	void SceneRenderNodes::doUpdateDescriptorsCounts( Pass const & pass
		, BillboardBase * billboard
		, Submesh const * submesh
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		m_allDescriptorCounts.update( pass.getTexturesMask().size()
			, billboard
			, submesh
			, mesh
			, skeleton );
	}

	void SceneRenderNodes::doUpdateNode( SubmeshRenderNode & node
		, ModelBufferConfiguration * modelBufferData )
	{
		fillEntry( node.passNode.pass
			, *node.instance.getParent()
			, node.instance
			, modelBufferData[node.getId() - 1u] );
	}

	void SceneRenderNodes::doUpdateNode( BillboardRenderNode & node
		, ModelBufferConfiguration * modelBufferData
		, BillboardUboConfiguration * billboardBufferData )
	{
		fillEntry( node.passNode.pass
			, *node.instance.getNode()
			, node.instance
			, modelBufferData[node.getId() - 1u] );

		auto & billboardData = billboardBufferData[node.getId() - 1u];
		billboardData.dimensions = node.instance.getDimensions();
	}

	//*************************************************************************************************
}
