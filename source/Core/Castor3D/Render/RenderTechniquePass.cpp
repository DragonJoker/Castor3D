#include "Castor3D/Render/RenderTechniquePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>

#include <RenderGraph/RecordContext.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d, RenderTechniquePass )

namespace castor3d
{
	//*************************************************************************************************

	void bindImage( VkImageView view
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		CU_Require( view != VkImageView{} );
		writes.push_back( { index++
			, 0u
			, VkDescriptorType{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE }
			, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ VK_NULL_HANDLE
				, view
				, VK_IMAGE_LAYOUT_GENERAL } } } );
	}

	void bindImage( ashes::ImageView const & view
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		bindImage( VkImageView( view )
			, writes
			, index );
	}

	void bindImage( crg::RunnableGraph & graph
		, crg::ImageViewId const & view
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		bindImage( graph.createImageView( view )
			, writes
			, index );
	}

	void bindTexture( VkImageView view
		, VkSampler sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		CU_Require( view != VkImageView{} );
		CU_Require( sampler != VkSampler{} );
		writes.push_back( { index++
			, 0u
			, VkDescriptorType{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
			, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ sampler
				, view
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } } );
	}

	void bindTexture( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		bindTexture( VkImageView( view )
			, VkSampler( sampler )
			, writes
			, index );
	}

	void bindTexture( crg::RunnableGraph & graph
		, crg::ImageViewId const & view
		, VkSampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		bindTexture( graph.createImageView( view )
			, sampler
			, writes
			, index );
	}

	void bindBuffer( VkBuffer buffer
		, VkDeviceSize offset
		, VkDeviceSize range
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		writes.push_back( { index++
			, 0u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ buffer
				, offset
				, range } } } );
	}

	void bindBuffer( ashes::BufferBase const & buffer
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		bindBuffer( buffer
			, 0u
			, buffer.getSize()
			, writes
			, index );
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( RenderTechnique * parent
		, Scene const & scene
		, bool outputScattering )
		: m_parent{ parent }
		, m_scene{ scene }
		, m_outputScattering{ outputScattering }
	{
	}

	Engine * RenderTechniquePass::getEngine()const
	{
		return m_scene.getOwner();
	}

	DebugConfig & RenderTechniquePass::getDebugConfig()const
	{
		return m_scene.getDebugConfig();
	}

	//*************************************************************************************************

	RenderTechniqueNodesPass::RenderTechniqueNodesPass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & typeName
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, typeName
			, std::move( targetImage )
			, std::move( targetDepth )
			, renderPassDesc }
		, RenderTechniquePass{ parent, renderPassDesc.m_culler.getScene(), techniquePassDesc.m_outputScattering }
		, m_camera{ renderPassDesc.m_culler.hasCamera() ? &renderPassDesc.m_culler.getCamera() : nullptr }
		, m_shaderFlags{ techniquePassDesc.m_shaderFlags }
		, m_ssaoConfig{ techniquePassDesc.m_ssaoConfig }
		, m_ssao{ techniquePassDesc.m_ssao }
		, m_indirectLighting{ techniquePassDesc.m_indirectLighting }
		, m_clustersConfig{ techniquePassDesc.m_clustersConfig }
	{
	}

	void RenderTechniqueNodesPass::accept( RenderTechniqueVisitor & visitor )
	{
	}

	void RenderTechniqueNodesPass::update( CpuUpdater & updater )
	{
		RenderNodesPass::update( updater );
	}

	PipelineFlags RenderTechniqueNodesPass::createPipelineFlags( PassComponentCombine components
		, BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, RenderPassTypeID renderPassTypeId
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureCombine const & textures
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, uint32_t passLayerIndex
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const
	{
		return RenderNodesPass::createPipelineFlags( std::move( components )
			, colourBlendMode
			, alphaBlendMode
			, renderPassTypeId
			, lightingModelId
			, backgroundModelId
			, alphaFunc
			, blendAlphaFunc
			, textures
			, submeshFlags
			, programFlags
			, sceneFlags
			, topology
			, isFrontCulled
			, passLayerIndex
			, morphTargets );
	}

	ProgramFlags RenderTechniqueNodesPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	SceneFlags RenderTechniqueNodesPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		if ( !m_indirectLighting.vctConfigUbo || !m_indirectLighting.vctFirstBounce || !m_indirectLighting.vctSecondaryBounce )
		{
			remFlag( flags, SceneFlag::eVoxelConeTracing );
		}

		if ( !m_indirectLighting.lpvConfigUbo || !m_indirectLighting.lpvResult )
		{
			remFlag( flags, SceneFlag::eLpvGI );
		}

		if ( !m_indirectLighting.llpvConfigUbo || !m_indirectLighting.lpvResult )
		{
			remFlag( flags, SceneFlag::eLayeredLpvGI );
		}

		return flags;
	}

	void RenderTechniqueNodesPass::doAddEnvBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++ // c3d_mapEnvironment
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	void RenderTechniqueNodesPass::doAddGIBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		addGIBindings( doAdjustSceneFlags( m_scene.getFlags() )
			, m_indirectLighting
			, bindings
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, index );
	}

	void RenderTechniqueNodesPass::doAddPassSpecificsBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		getEngine()->addSpecificsBuffersBindings( bindings
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, index );
	}

	void RenderTechniqueNodesPass::doAddEnvDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		bindTexture( m_scene.getEnvironmentMap().getColourId().sampledView
			, *m_scene.getEnvironmentMap().getColourId().sampler
			, descriptorWrites
			, index );
	}

	void RenderTechniqueNodesPass::doAddGIDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		addGIDescriptor( doAdjustSceneFlags( m_scene.getFlags() )
			, m_indirectLighting
			, descriptorWrites
			, index );
	}

	void RenderTechniqueNodesPass::doAddPassSpecificsDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		getEngine()->addSpecificsBuffersDescriptors( descriptorWrites, index );
	}

	void RenderTechniqueNodesPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		doAddPassSpecificsBindings( flags, bindings, index );
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( VK_SHADER_STAGE_FRAGMENT_BIT
			, index++ ) );

		if ( hasSsao() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) ); // c3d_mapOcclusion
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf

		doAddShadowBindings( m_scene, flags, bindings, index );
		doAddEnvBindings( flags, bindings, index );
		doAddBackgroundBindings( m_scene, bindings, index );
		doAddGIBindings( flags, bindings, index );

		if ( m_parent )
		{
			doAddClusteredLightingBindings( m_parent->getRenderTarget(), flags, bindings, index );
		}
	}

	void RenderTechniqueNodesPass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		doAddPassSpecificsDescriptor( flags, descriptorWrites, index );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index++ ) );

		if ( hasSsao() )
		{
			bindTexture( m_ssao->wholeView
				, *m_ssao->sampler
				, descriptorWrites
				, index );
		}

		bindTexture( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().wholeView
			, *getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampler
			, descriptorWrites
			, index );
		doAddShadowDescriptor( m_scene, flags, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddEnvDescriptor( flags, descriptorWrites, index );
		doAddBackgroundDescriptor( m_scene, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( flags, descriptorWrites, index );

		if ( m_parent )
		{
			doAddClusteredLightingDescriptor( m_parent->getRenderTarget(), flags, descriptorWrites, index );
		}
	}

	ashes::PipelineDepthStencilStateCreateInfo RenderTechniqueNodesPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		if ( flags.writeEnvironmentMap() )
		{
			return ashes::PipelineDepthStencilStateCreateInfo{ 0u
				, VK_TRUE
				, checkFlag( m_filters, RenderFilter::eAlphaBlend )
				, VK_COMPARE_OP_GREATER };
		}
		else
		{
			return ashes::PipelineDepthStencilStateCreateInfo{ 0u
				, VK_TRUE
				, checkFlag( m_filters, RenderFilter::eAlphaBlend )
				, VK_COMPARE_OP_GREATER_OR_EQUAL };
		}
	}

	ashes::PipelineColorBlendStateCreateInfo RenderTechniqueNodesPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		uint32_t count = 1u
			+ ( flags.writeVelocity() ? 1u : 0u )
			+ ( m_outputScattering ? 1u : 0u )
			+ ( ( m_deferredLightingFilter == DeferredLightingFilter::eDeferLighting ) ? 1u : 0u );
		return RenderNodesPass::createBlendState( flags.colourBlendMode
			, flags.alphaBlendMode
			, count );
	}
}
