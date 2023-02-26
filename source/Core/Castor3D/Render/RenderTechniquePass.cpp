#include "Castor3D/Render/RenderTechniquePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
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

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderTechniquePass )

namespace castor3d
{
	//*************************************************************************************************

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
		, Scene const & scene )
		: m_parent{ parent }
		, m_scene{ scene }
	{
	}

	Engine * RenderTechniquePass::getEngine()const
	{
		return m_scene.getOwner();
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
		, RenderTechniquePass{ parent, renderPassDesc.m_culler.getScene() }
		, m_camera{ renderPassDesc.m_culler.hasCamera() ? &renderPassDesc.m_culler.getCamera() : nullptr }
		, m_shaderFlags{ techniquePassDesc.m_shaderFlags }
		, m_ssaoConfig{ techniquePassDesc.m_ssaoConfig }
		, m_ssao{ techniquePassDesc.m_ssao }
		, m_lpvConfigUbo{ techniquePassDesc.m_lpvConfigUbo }
		, m_llpvConfigUbo{ techniquePassDesc.m_llpvConfigUbo }
		, m_vctConfigUbo{ techniquePassDesc.m_vctConfigUbo }
		, m_lpvResult{ techniquePassDesc.m_lpvResult }
		, m_llpvResult{ techniquePassDesc.m_llpvResult }
		, m_vctFirstBounce{ techniquePassDesc.m_vctFirstBounce }
		, m_vctSecondaryBounce{ techniquePassDesc.m_vctSecondaryBounce }
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
		if ( !m_vctConfigUbo || !m_vctFirstBounce || !m_vctSecondaryBounce )
		{
			remFlag( flags, SceneFlag::eVoxelConeTracing );
		}

		if ( !m_lpvConfigUbo || !m_lpvResult )
		{
			remFlag( flags, SceneFlag::eLpvGI );
		}

		if ( !m_llpvConfigUbo || !m_lpvResult )
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
		auto sceneFlags = doAdjustSceneFlags( m_scene.getFlags() );

		if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapVoxelsFirstBounce
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapVoxelsSecondaryBounce
		}
		else
		{
			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationR
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationG
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationB
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvResult );

				for ( size_t i = 0u; i < m_llpvResult->size(); ++i )
				{
					bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationRn
					bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationGn
					bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationBn
				}
			}
		}
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
		auto sceneFlags = doAdjustSceneFlags( m_scene.getFlags() );

		if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( m_vctConfigUbo );
			CU_Require( m_vctFirstBounce );
			CU_Require( m_vctSecondaryBounce );
			descriptorWrites.push_back( m_vctConfigUbo->getDescriptorWrite( index++ ) );
			bindTexture( m_vctFirstBounce->wholeView
				, *m_vctFirstBounce->sampler
				, descriptorWrites
				, index );
			bindTexture( m_vctSecondaryBounce->wholeView
				, *m_vctSecondaryBounce->sampler
				, descriptorWrites
				, index );
		}
		else
		{
			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvConfigUbo );
				descriptorWrites.push_back( m_lpvConfigUbo->getDescriptorWrite( index++ ) );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvConfigUbo );
				descriptorWrites.push_back( m_llpvConfigUbo->getDescriptorWrite( index++ ) );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvResult );
				auto & lpv = *m_lpvResult;
				bindTexture( lpv[LpvTexture::eR].wholeView
					, *lpv[LpvTexture::eR].sampler
					, descriptorWrites
					, index );
				bindTexture( lpv[LpvTexture::eG].wholeView
					, *lpv[LpvTexture::eG].sampler
					, descriptorWrites
					, index );
				bindTexture( lpv[LpvTexture::eB].wholeView
					, *lpv[LpvTexture::eB].sampler
					, descriptorWrites
					, index );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvResult );

				for ( auto & plpv : *m_llpvResult )
				{
					auto & lpv = *plpv;
					bindTexture( lpv[LpvTexture::eR].wholeView
						, *lpv[LpvTexture::eR].sampler
						, descriptorWrites
						, index );
					bindTexture( lpv[LpvTexture::eG].wholeView
						, *lpv[LpvTexture::eG].sampler
						, descriptorWrites
						, index );
					bindTexture( lpv[LpvTexture::eB].wholeView
						, *lpv[LpvTexture::eB].sampler
						, descriptorWrites
						, index );
				}
			}
		}
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
		bindings.emplace_back( m_scene.getLightCache().createLayoutBinding( index++ ) );

		if ( m_ssao )
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
		doAddBackgroundBindings( m_scene, flags, bindings, index );
		doAddGIBindings( flags, bindings, index );
	}

	void RenderTechniqueNodesPass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		doAddPassSpecificsDescriptor( flags, descriptorWrites, index );
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( index++ ) );

		if ( m_ssao )
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
		doAddShadowDescriptor( m_scene, flags, descriptorWrites, shadowMaps, index );
		doAddEnvDescriptor( flags, descriptorWrites, index );
		doAddBackgroundDescriptor( m_scene, flags, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( flags, descriptorWrites, index );
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
				, VK_FALSE
				, ( checkFlag( m_filters, RenderFilter::eAlphaBlend )
					? VK_COMPARE_OP_EQUAL
					: VK_COMPARE_OP_GREATER_OR_EQUAL ) };
		}
	}

	ashes::PipelineColorBlendStateCreateInfo RenderTechniqueNodesPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}
}
