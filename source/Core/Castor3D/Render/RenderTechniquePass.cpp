#include "Castor3D/Render/RenderTechniquePass.hpp"

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
		writes.emplace_back( index
			, 0u
			, VkDescriptorType{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE }
			, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ VK_NULL_HANDLE
				, view
				, VK_IMAGE_LAYOUT_GENERAL } } );
		++index;
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
		writes.emplace_back( index
			, 0u
			, VkDescriptorType{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
			, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ sampler
				, view
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
		++index;
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
		writes.emplace_back( index
			, 0u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ buffer
				, offset
				, range } } );
		++index;
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

	Engine * RenderTechniquePass::getEngine()const noexcept
	{
		return m_scene.getOwner();
	}

	IndirectLightingData const & RenderTechniquePass::getIndirectLighting()const noexcept
	{
		static IndirectLightingData const dummy{};
		return m_parent
			? m_parent->getIndirectLighting()
			: dummy;
	}

	bool RenderTechniquePass::areDebugTargetsEnabled()const noexcept
	{
		return m_scene.getEngine()->areDebugTargetsEnabled();
	}

	DebugConfig & RenderTechniquePass::getDebugConfig()const noexcept
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
		doAccept( visitor );
	}

	void RenderTechniqueNodesPass::update( CpuUpdater & updater )
	{
		RenderNodesPass::update( updater );
	}

	PipelineFlags RenderTechniqueNodesPass::createPipelineFlags( PassComponentCombine passComponents
		, SubmeshComponentCombine submeshComponents
		, BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, RenderPassTypeID renderPassTypeId
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureCombine const & textures
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, uint32_t passLayerIndex
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, SubmeshRenderData * submeshData )const
	{
		return RenderNodesPass::createPipelineFlags( std::move( passComponents )
			, std::move( submeshComponents )
			, colourBlendMode
			, alphaBlendMode
			, renderPassTypeId
			, lightingModelId
			, backgroundModelId
			, alphaFunc
			, blendAlphaFunc
			, textures
			, programFlags
			, sceneFlags
			, topology
			, isFrontCulled
			, passLayerIndex
			, morphTargets
			, submeshData );
	}

	void RenderTechniqueNodesPass::doAccept( RenderTechniqueVisitor & visitor )
	{
		RenderNodesPass::doAccept( visitor );
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

		if ( !m_indirectLighting.llpvConfigUbo || !m_indirectLighting.llpvResult )
		{
			remFlag( flags, SceneFlag::eLayeredLpvGI );
		}

		return flags;
	}

	void RenderTechniqueNodesPass::doAddEnvBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index // c3d_mapEnvironment
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		++index;
	}

	void RenderTechniqueNodesPass::doAddGIBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		addGIBindings( doAdjustSceneFlags( m_scene.getFlags() )
			, m_indirectLighting
			, bindings
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, index );
	}

	void RenderTechniqueNodesPass::doAddPassSpecificsBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		getEngine()->addSpecificsBuffersBindings( bindings
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, index );
	}

	void RenderTechniqueNodesPass::doAddEnvDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		bindTexture( m_scene.getEnvironmentMap().getColourId().sampledView
			, *m_scene.getEnvironmentMap().getColourId().sampler
			, descriptorWrites
			, index );
	}

	void RenderTechniqueNodesPass::doAddGIDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		addGIDescriptor( doAdjustSceneFlags( m_scene.getFlags() )
			, m_indirectLighting
			, descriptorWrites
			, index );
	}

	void RenderTechniqueNodesPass::doAddPassSpecificsDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		getEngine()->addSpecificsBuffersDescriptors( descriptorWrites, index );
	}

	void RenderTechniqueNodesPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		doAddPassSpecificsBindings( bindings, index );
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( VK_SHADER_STAGE_FRAGMENT_BIT
			, index ) );
		++index;

		if ( hasSsao() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) ); // c3d_mapOcclusion
			++index;
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf
		++index;

		doAddShadowBindings( m_scene, bindings, index );
		doAddEnvBindings( bindings, index );
		doAddBackgroundBindings( m_scene, bindings, index );
		doAddGIBindings( bindings, index );

		if ( m_parent )
		{
			doAddClusteredLightingBindings( m_parent->getRenderTarget(), bindings, index );
		}
	}

	void RenderTechniqueNodesPass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		doAddPassSpecificsDescriptor( descriptorWrites, index );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index ) );
		++index;

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
		doAddShadowDescriptor( m_scene, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddEnvDescriptor( descriptorWrites, index );
		doAddBackgroundDescriptor( m_scene, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( descriptorWrites, index );

		if ( m_parent )
		{
			doAddClusteredLightingDescriptor( m_parent->getRenderTarget(), descriptorWrites, index );
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
