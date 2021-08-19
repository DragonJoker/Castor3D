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
			size_t hash = texturesCount;

			if ( billboard )
			{
				hash |= 0x00001000;
			}
			else
			{
				hash |= 0x00002000;

				if ( mesh )
				{
					hash |= 0x00004000;
				}

				if ( skeleton )
				{
					hash |= 0x00008000;
				}
			}

			return hash;
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

		size_t makeHash( SceneNode & sceneNode
			, Submesh & data
			, Geometry & instance )
		{
			auto hash = std::hash< SceneNode * >{}( &sceneNode );
			hash = castor::hashCombinePtr( hash, data );
			hash = castor::hashCombinePtr( hash, instance );
			return hash;
		}

		size_t makeHash( SceneNode & sceneNode
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
			if ( !node.uboDescriptorSet )
			{
				node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName( node ) + "Ubo"
					, layout
					, RenderPipeline::eBuffers );
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
			if ( !node.texDescriptorSet )
			{
				node.texDescriptorSet = descriptorPool.createDescriptorSet( getName( node ) + "Tex"
					, layout
					, RenderPipeline::eTextures );
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
			, ashes::DescriptorPool const & descriptorPool
			, std::unordered_map< size_t, SceneRenderNodes::DescriptorSetLayouts > const & descriptorLayouts
			, NodeT & node )
		{
			size_t hash = makeLayoutHash( node );
			auto layouts = descriptorLayouts.find( hash );
			CU_Require( layouts != descriptorLayouts.end() );

			if ( !layouts->second.ubo->getBindings().empty() )
			{
				doInitialiseUboDescriptor( engine
					, *layouts->second.ubo
					, descriptorPool
					, node );
			}

			if ( !layouts->second.tex->getBindings().empty() )
			{
				doInitialiseTextureDescriptor( *layouts->second.tex
					, descriptorPool
					, node );
			}

		}
	}

	//*********************************************************************************************

	SceneRenderNodes::SceneRenderNodes( Scene const & scene )
		: castor::OwnedBy< Scene const >{ scene }
	{
	}

	void SceneRenderNodes::initialiseNodes( RenderDevice const & device )
	{
		doInitialiseDescriptorPool( device );

		for ( auto & node : m_submeshNodes )
		{
			doInitialiseNode( *getOwner()->getEngine()
				, *m_descriptorPool
				, m_descriptorLayouts
				, *node.second );
		}

		for ( auto & node : m_billboardNodes )
		{
			doInitialiseNode( *getOwner()->getEngine() 
				, *m_descriptorPool
				, m_descriptorLayouts
				, *node.second );
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
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto it = m_submeshNodes.emplace( makeHash( sceneNode, data, instance ), nullptr );

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
		auto lock( castor::makeUniqueLock( m_nodesMutex ) );
		auto it = m_billboardNodes.emplace( makeHash( sceneNode, instance ), nullptr );

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
		for ( auto & node : m_submeshNodes )
		{
			doUpdateNode( *node.second );
		}

		for ( auto & node : m_billboardNodes )
		{
			doUpdateNode( *node.second );
		}
	}

	ashes::DescriptorSetLayoutCRefArray SceneRenderNodes::doGetDescriptorSetLayouts( Pass const & pass
		, BillboardBase const * billboard
		, Submesh const * submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		auto lock( castor::makeUniqueLock( m_layoutsMutex ) );
		auto textures = pass.getTexturesMask();
		size_t hash = makeLayoutHash( textures.size(), billboard, submesh, mesh, skeleton );
		auto ires = m_descriptorLayouts.emplace( hash, DescriptorSetLayouts{} );

		if ( ires.second )
		{
			auto & engine = *pass.getOwner()->getEngine();
			auto & device = engine.getRenderSystem()->getRenderDevice();
			ires.first->second.ubo = device->createDescriptorSetLayout( doCreateUboBindings( engine
				, textures.size()
				, billboard
				, submesh
				, ( billboard
					? false
					: submesh->getInstantiation().isInstanced( pass.getOwner()->shared_from_this() ) )
				, mesh
				, skeleton ) );
			ires.first->second.tex = device->createDescriptorSetLayout( doCreateTextureBindings( textures.size() ) );
		}

		ashes::DescriptorSetLayoutCRefArray result;
		result.emplace_back( *ires.first->second.ubo );
		result.emplace_back( *ires.first->second.tex );
		return result;
	}

	void SceneRenderNodes::doUpdateDescriptorsCounts( Pass const & pass
		, BillboardBase * billboard
		, Submesh const * submesh
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		m_descriptorCounts.texelBuffers++; // Materials TBO
		m_descriptorCounts.storageBuffers++; // Materials SSBO
		m_descriptorCounts.texelBuffers++; // Textures Configs TBO
		m_descriptorCounts.storageBuffers++; // Textures Configs SSBO
		m_descriptorCounts.uniformBuffers++; // Model UBO
		m_descriptorCounts.uniformBuffers++; // ModelInstances UBO

		if ( billboard )
		{
			m_descriptorCounts.uniformBuffers++; // Billboard UBO
		}
		else
		{
			if ( mesh )
			{
				m_descriptorCounts.uniformBuffers++; // Morphing UBO
			}

			if ( skeleton )
			{
				if ( submesh->getInstantiatedBones().hasInstancedBonesBuffer() )
				{
					if ( getOwner()->getEngine()->getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
					{
						m_descriptorCounts.storageBuffers++; // Instantiated Skinning SSBO
					}
					else
					{
						m_descriptorCounts.texelBuffers++; // Instantiated Skinning TBO
					}
				}
				else
				{
					m_descriptorCounts.uniformBuffers++; // Skinning UBO
				}
			}
		}

		auto textures = pass.getTexturesMask();
		m_descriptorCounts.samplers += textures.size();
	}

	void SceneRenderNodes::doInitialiseDescriptorPool( RenderDevice const & device )
	{
		auto newSize = uint32_t( m_submeshNodes.size() + m_billboardNodes.size() );

		if ( newSize
			&& ( m_currentPoolSize < newSize
				|| !m_descriptorPool ) )
		{
			ashes::VkDescriptorPoolSizeArray sizes;

			if ( m_descriptorCounts.uniformBuffers )
			{
				sizes.push_back( { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_descriptorCounts.uniformBuffers } );
			}

			if ( m_descriptorCounts.storageBuffers )
			{
				sizes.push_back( { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, m_descriptorCounts.storageBuffers } );
			}

			if ( m_descriptorCounts.samplers )
			{
				sizes.push_back( { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_descriptorCounts.samplers } );
			}

			if ( m_descriptorCounts.texelBuffers )
			{
				sizes.push_back( { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, m_descriptorCounts.texelBuffers } );
			}

			if ( m_descriptorPool )
			{
				for ( auto & node : m_submeshNodes )
				{
					node.second->uboDescriptorSet.reset();
					node.second->texDescriptorSet.reset();
				}

				for ( auto & node : m_billboardNodes )
				{
					node.second->uboDescriptorSet.reset();
					node.second->texDescriptorSet.reset();
				}

				m_descriptorPool.reset();
			}

			m_descriptorPool = device->createDescriptorPool( VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
				, newSize * 2
				, std::move( sizes ) );
			m_currentPoolSize = newSize;
		}
	}

	void SceneRenderNodes::doUpdateNode( SubmeshRenderNode & node )
	{
	}

	void SceneRenderNodes::doUpdateNode( BillboardRenderNode & node )
	{
	}

	//*************************************************************************************************
}
