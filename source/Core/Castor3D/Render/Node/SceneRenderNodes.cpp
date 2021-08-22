#include "Castor3D/Render/Node/SceneRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPass.hpp"
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
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
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
			enum BitVal : uint16_t
			{
				BillboadUBO = 0x0001 << 0,
				MorphingUBO = 0x0001 << 1,
				SkinningUBO = 0x0001 << 2,
				InstSkinSSBO = 0x0001 << 3,
				InstSkinTBO = 0x0001 << 4,
			};

			Engine * engine{};
			uint16_t spec{};

			if ( billboard )
			{
				engine = billboard->getParentScene().getEngine();
				spec |= BillboadUBO; // Billboard UBO
			}
			else
			{
				engine = submesh->getOwner()->getEngine();

				if ( mesh )
				{
					spec |= MorphingUBO; // Morphing UBO
				}

				if ( skeleton )
				{
					if ( submesh->getInstantiatedBones().hasInstancedBonesBuffer() )
					{
						if ( engine->getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
						{
							spec |= InstSkinSSBO; // Instantiated Skinning SSBO
						}
						else
						{
							spec |= InstSkinTBO; // Instantiated Skinning TBO
						}
					}
					else
					{
						spec |= SkinningUBO; // Skinning UBO
					}
				}
			}

			size_t result{ spec };
			result <<= 16u;
			result += texturesCount;
			return result;
		}

		size_t makeLayoutHash( SubmeshRenderNode const & node )
		{
			return makeLayoutHash( node.passNode.pass.getTexturesMask().size()
				, nullptr
				, &node.data
				, node.mesh
				, node.skeleton );
		}

		size_t makeLayoutHash( BillboardRenderNode const & node )
		{
			return makeLayoutHash( node.passNode.pass.getTexturesMask().size()
				, &node.data
				, nullptr
				, nullptr
				, nullptr );
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

		//*****************************************************************************************

		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( Engine const & engine
			, size_t texturesCount
			, BillboardBase const * billboard
			, Submesh const * submesh
			, bool instantiated
			, AnimatedMesh const * mesh
			, AnimatedSkeleton const * skeleton )
		{
			ashes::VkDescriptorSetLayoutBindingArray uboBindings;
			uboBindings.emplace_back( engine.getMaterialCache().getPassBuffer().createLayoutBinding( uint32_t( NodeUboIdx::eMaterials ) ) );
			uboBindings.emplace_back( engine.getMaterialCache().getTextureBuffer().createLayoutBinding( uint32_t( NodeUboIdx::eTextures ) ) );
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eModel )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ( VK_SHADER_STAGE_FRAGMENT_BIT
					| VK_SHADER_STAGE_GEOMETRY_BIT
					| VK_SHADER_STAGE_VERTEX_BIT ) ) );

			if ( billboard )
			{
				uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eBillboard )
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT ) );
			}
			else
			{
				if ( skeleton )
				{
					if ( instantiated )
					{
						if ( engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
						{
							uboBindings.push_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eSkinningSsbo )
								, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
								, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT ) );
						}
						else
						{
							uboBindings.push_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eSkinningSsbo )
								, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
								, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT ) );
						}
					}

					uboBindings.push_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eSkinningUbo )
						, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
						, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT ) );
				}

				if ( mesh )
				{
					uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eMorphing )
						, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
						, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT ) );
				}
			}

			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eModelInstances )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
			return uboBindings;
		}

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
		void doInitialiseUboDescriptor( Engine & engine
			, ashes::DescriptorSetLayout const & layout
			, ashes::DescriptorPool const & descriptorPool
			, NodeT & node )
		{
			if ( node.uboDescriptorSet )
			{
				auto & descriptorSet = *node.uboDescriptorSet;
				engine.getMaterialCache().getPassBuffer().createBinding( descriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eMaterials ) ) );
				engine.getMaterialCache().getTextureBuffer().createBinding( descriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eTextures ) ) );
				CU_Require( node.modelUbo );
				node.modelUbo.createSizedBinding( descriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eModel ) ) );

				if constexpr ( std::is_same_v< NodeT, SubmeshRenderNode > )
				{
					SubmeshRenderNode & submeshNode = node;

					if ( submeshNode.mesh )
					{
						submeshNode.morphingUbo.createSizedBinding( descriptorSet
							, layout.getBinding( uint32_t( NodeUboIdx::eMorphing ) ) );
					}

					if ( submeshNode.skeleton )
					{
						if ( submeshNode.data.getInstantiation().isInstanced( submeshNode.passNode.pass.getOwner()->shared_from_this() ) )
						{
							submeshNode.data.getInstantiatedBones().getInstancedBonesBuffer().createBinding( descriptorSet
								, layout.getBinding( uint32_t( NodeUboIdx::eSkinningSsbo ) ) );
						}

						submeshNode.skinningUbo.createSizedBinding( descriptorSet
							, layout.getBinding( uint32_t( NodeUboIdx::eSkinningUbo ) ) );
					}
				}
				else
				{
					node.billboardUbo.createSizedBinding( descriptorSet
						, layout.getBinding( uint32_t( NodeUboIdx::eBillboard ) ) );
				}

				if ( node.modelInstancesUbo )
				{
					node.modelInstancesUbo.createSizedBinding( descriptorSet
						, layout.getBinding( uint32_t( NodeUboIdx::eModelInstances ) ) );
				}

				descriptorSet.update();
			}
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
			if ( !descriptorLayout.buf->getBindings().empty() )
			{
				doInitialiseUboDescriptor( engine
					, *descriptorLayout.buf
					, *pools.buf
					, node );
			}

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

	static constexpr uint32_t MaxPoolSize = 50u;

	SceneRenderNodes::DescriptorSetPools::DescriptorSetPools( RenderDevice const & device
		, SceneRenderNodes::DescriptorCounts const & counts )
	{
		ashes::VkDescriptorPoolSizeArray bufSizes;
		ashes::VkDescriptorPoolSizeArray texSizes;

		if ( counts.uniformBuffers )
		{
			bufSizes.push_back( { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, counts.uniformBuffers } );
		}

		if ( counts.storageBuffers )
		{
			bufSizes.push_back( { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, counts.storageBuffers } );
		}

		if ( counts.texelBuffers )
		{
			bufSizes.push_back( { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, counts.texelBuffers } );
		}

		if ( counts.samplers )
		{
			texSizes.push_back( { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, counts.samplers } );
		}

		if ( !bufSizes.empty() )
		{
			buf = device->createDescriptorPool( VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
				, MaxPoolSize
				, std::move( bufSizes ) );
		}

		if ( !texSizes.empty() )
		{
			tex = device->createDescriptorPool( VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
				, MaxPoolSize
				, std::move( texSizes ) );
		}
	}

	//*********************************************************************************************

	SceneRenderNodes::DescriptorCounts::DescriptorCounts( bool hasSSBO
		, size_t textureCount
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		update( hasSSBO
			, textureCount
			, billboard
			, submesh
			, mesh
			, skeleton );
	}

	void SceneRenderNodes::DescriptorCounts::update( bool hasSSBO
		, size_t textureCount
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		texelBuffers++; // Materials TBO
		storageBuffers++; // Materials SSBO
		texelBuffers++; // Textures Configs TBO
		storageBuffers++; // Textures Configs SSBO
		uniformBuffers++; // Model UBO
		uniformBuffers++; // ModelInstances UBO

		if ( billboard )
		{
			uniformBuffers++; // Billboard UBO
		}
		else
		{
			if ( mesh )
			{
				uniformBuffers++; // Morphing UBO
			}

			if ( skeleton )
			{
				if ( submesh->getInstantiatedBones().hasInstancedBonesBuffer() )
				{
					if ( hasSSBO )
					{
						storageBuffers++; // Instantiated Skinning SSBO
					}
					else
					{
						texelBuffers++; // Instantiated Skinning TBO
					}
				}
				else
				{
					uniformBuffers++; // Skinning UBO
				}
			}
		}

		samplers += textureCount;
	}
	
	//*********************************************************************************************

	SceneRenderNodes::DescriptorPools::DescriptorPools( Engine & engine
		, DescriptorSetLayouts * layouts
		, DescriptorCounts * counts )
		: m_engine{ engine }
		, m_layouts{ layouts }
		, m_counts{ counts }
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
				, *m_counts );
			m_available += MaxPoolSize;
		}

		--m_available;
		doAllocateBuf( m_pools.back(), node, it->second );
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
				, *m_counts );
		}

		--m_available;
		doAllocateBuf( m_pools.back(), node, it->second );
		doAllocateTex( m_pools.back(), node, it->second );
	}

	void SceneRenderNodes::DescriptorPools::deallocate( SubmeshRenderNode & node )
	{
		auto it = m_allocated.find( intptr_t( &node ) );

		if ( it != m_allocated.end() )
		{
			m_allocated.erase( it );
			node.uboDescriptorSet = nullptr;
			node.texDescriptorSet = nullptr;
		}
	}

	void SceneRenderNodes::DescriptorPools::deallocate( BillboardRenderNode & node )
	{
		auto it = m_allocated.find( intptr_t( &node ) );

		if ( it != m_allocated.end() )
		{
			m_allocated.erase( it );
			node.uboDescriptorSet = nullptr;
			node.texDescriptorSet = nullptr;
		}
	}

	void SceneRenderNodes::DescriptorPools::doAllocateBuf( DescriptorSetPools const & pools
		, SubmeshRenderNode & node
		, SceneRenderNodes::DescriptorPools::NodeSet & allocated )
	{
		if ( !m_layouts->buf->getBindings().empty() )
		{
			allocated.bufPool = &( *pools.buf );
			allocated.bufSet = pools.buf->createDescriptorSet( getName( node ) + "Ubo"
				, *m_layouts->buf
				, RenderPipeline::eBuffers );
			node.uboDescriptorSet = allocated.bufSet.get();
			doInitialiseUboDescriptor( m_engine
				, *m_layouts->buf
				, *pools.buf
				, node );
		}
	}

	void SceneRenderNodes::DescriptorPools::doAllocateBuf( DescriptorSetPools const & pools
		, BillboardRenderNode & node
		, SceneRenderNodes::DescriptorPools::NodeSet & allocated )
	{
		if ( !m_layouts->buf->getBindings().empty() )
		{
			allocated.bufPool = &( *pools.buf );
			allocated.bufSet = pools.buf->createDescriptorSet( getName( node ) + "Ubo"
				, *m_layouts->buf
				, RenderPipeline::eBuffers );
			node.uboDescriptorSet = allocated.bufSet.get();
			doInitialiseUboDescriptor( m_engine
				, *m_layouts->buf
				, *pools.buf
				, node );
		}
	}

	void SceneRenderNodes::DescriptorPools::doAllocateTex( DescriptorSetPools const & pools
		, SubmeshRenderNode & node
		, NodeSet & allocated )
	{
		if ( !m_layouts->tex->getBindings().empty() )
		{
			allocated.texPool = &( *pools.tex );
			allocated.texSet = pools.tex->createDescriptorSet( getName( node ) + "Tex"
				, *m_layouts->tex
				, RenderPipeline::eTextures );
			node.texDescriptorSet = allocated.texSet.get();
			doInitialiseTextureDescriptor( *m_layouts->tex
				, *pools.tex
				, node );
		}
	}

	void SceneRenderNodes::DescriptorPools::doAllocateTex( DescriptorSetPools const & pools
		, BillboardRenderNode & node
		, NodeSet & allocated )
	{
		if ( !m_layouts->tex->getBindings().empty() )
		{
			allocated.texPool = &( *pools.tex );
			allocated.texSet = pools.tex->createDescriptorSet( getName( node ) + "Tex"
				, *m_layouts->tex
				, RenderPipeline::eTextures );
			node.texDescriptorSet = allocated.texSet.get();
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
		, AnimatedSkeleton const * skeleton )
	{
		auto & device = engine.getRenderSystem()->getRenderDevice();
		buf = device->createDescriptorSetLayout( doCreateUboBindings( engine
			, texturesCount
			, billboard
			, submesh
			, instanced
			, mesh
			, skeleton ) );
		tex = device->createDescriptorSetLayout( doCreateTextureBindings( texturesCount ) );
	}

	//*********************************************************************************************

	template<>
	SceneRenderNodes::DescriptorNodesT< SubmeshRenderNode  >::DescriptorNodesT( Engine & engine
		, size_t texturesCount
		, bool instanced
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
		: counts{ engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers )
			, texturesCount
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
			, skeleton }
		, pools{ engine, &layouts, &counts }
	{
	}
	
	template<>
	SceneRenderNodes::DescriptorNodesT< BillboardRenderNode  >::DescriptorNodesT( Engine & engine
		, size_t texturesCount
		, bool instanced
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
		: counts{ engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers )
			, texturesCount
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
			, skeleton }
		, pools{ engine, &layouts, &counts }
	{
	}

	//*********************************************************************************************

	SceneRenderNodes::SceneRenderNodes( Scene const & scene )
		: castor::OwnedBy< Scene const >{ scene }
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

	SubmeshRenderNode & SceneRenderNodes::createNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		auto & pass = passNode.pass;
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & pool = getNodesPool( pass.getTexturesMask().size()
			, data.getInstantiation().isInstanced( pass.getOwner()->shared_from_this() )
			, nullptr
			, &data
			, mesh
			, skeleton
			, *pass.getOwner()->getEngine()
			, m_submeshNodes );
		auto it = pool.nodes.emplace( makeNodeHash( sceneNode, data, instance ), nullptr );

		if ( it.second )
		{
			doUpdateDescriptorsCounts( passNode.pass, nullptr, &data, mesh, skeleton );
			it.first->second = castor::makeUnique< SubmeshRenderNode >( std::move( passNode )
				, std::move( modelBuffer )
				, std::move( modelInstancesBuffer )
				, buffers
				, sceneNode
				, data
				, instance );
			it.first->second->mesh = mesh;
			it.first->second->skeleton = skeleton;
		}

		return *it.first->second;
	}

	BillboardRenderNode & SceneRenderNodes::createNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & instance
		, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer )
	{
		auto & pass = passNode.pass;
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto & pool = getNodesPool( pass.getTexturesMask().size()
			, false
			, &instance
			, nullptr
			, nullptr
			, nullptr
			, *pass.getOwner()->getEngine()
			, m_billboardNodes );
		auto it = pool.nodes.emplace( makeNodeHash( sceneNode, instance ), nullptr );

		if ( it.second )
		{
			doUpdateDescriptorsCounts( passNode.pass, &instance, nullptr, nullptr, nullptr );
			it.first->second = castor::makeUnique< BillboardRenderNode >( std::move( passNode )
				, std::move( modelBuffer )
				, std::move( modelInstancesBuffer )
				, buffers
				, sceneNode
				, instance
				, billboardBuffer );
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
		for ( auto & nodes : m_submeshNodes )
		{
			for ( auto & node : nodes.second->nodes )
			{
				doUpdateNode( *node.second );
			}
		}

		for ( auto & nodes : m_billboardNodes )
		{
			for ( auto & node : nodes.second->nodes )
			{
				doUpdateNode( *node.second );
			}
		}
	}

	ashes::DescriptorSetLayoutCRefArray SceneRenderNodes::doGetDescriptorSetLayouts( Pass const & pass
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		DescriptorSetLayouts * layouts;

		if ( billboard )
		{
			auto & pool = getNodesPool( pass.getTexturesMask().size()
				, false
				, billboard
				, submesh
				, mesh
				, skeleton
				, *pass.getOwner()->getEngine()
				, m_billboardNodes );
			layouts = &pool.layouts;
		}
		else
		{
			auto & pool = getNodesPool( pass.getTexturesMask().size()
				, submesh->getInstantiation().isInstanced( pass.getOwner()->shared_from_this() )
				, billboard
				, submesh
				, mesh
				, skeleton
				, *pass.getOwner()->getEngine()
				, m_submeshNodes );
			layouts = &pool.layouts;
		}

		ashes::DescriptorSetLayoutCRefArray result;
		result.emplace_back( *layouts->buf );
		result.emplace_back( *layouts->tex );
		return result;
	}

	void SceneRenderNodes::doUpdateDescriptorsCounts( Pass const & pass
		, BillboardBase * billboard
		, Submesh const * submesh
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		m_allDescriptorCounts.update( getOwner()->getEngine()->getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers )
			, pass.getTexturesMask().size()
			, billboard
			, submesh
			, mesh
			, skeleton );
	}

	void SceneRenderNodes::doUpdateNode( SubmeshRenderNode & node )
	{
	}

	void SceneRenderNodes::doUpdateNode( BillboardRenderNode & node )
	{
	}

	//*************************************************************************************************
}
