#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

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
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderTechniquePass )

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	void bindTexture( VkImageView view
		, VkSampler sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		writes.push_back( { index++
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, { { sampler
				, view
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } } );
	}
	
	void bindTexture( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		writes.push_back( { index++
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, { { sampler
				, view
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } } );
	}

	void bindTexture( crg::RunnableGraph & graph
		, crg::ImageViewId const & view
		, VkSampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		auto texture = graph.createImageView( view );
		CU_Require( texture != VK_NULL_HANDLE );
		bindTexture( texture
			, sampler
			, writes
			, index );
	}

	void bindShadowMaps( crg::RunnableGraph & graph
		, PipelineFlags const & pipelineFlags
		, ShadowMapLightTypeArray const & shadowMaps
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
#if !C3D_DebugDisableShadowMaps
		for ( auto i = 0u; i < uint32_t( LightType::eCount ); ++i )
		{
			if ( checkFlag( pipelineFlags.sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << i ) ) )
			{
				for ( auto & shadowMapRef : shadowMaps[i] )
				{
					auto & result = shadowMapRef.first.get().getShadowPassResult();
					bindTexture( graph
						, result[SmTexture::eNormalLinear].sampledViewId
						, *result[SmTexture::eNormalLinear].sampler
						, writes
						, index );
					bindTexture( graph
						, result[SmTexture::eVariance].sampledViewId
						, *result[SmTexture::eVariance].sampler
						, writes
						, index );
				}
			}
		}
#endif
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, String const & category
		, String const & name
		, SceneRenderPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: SceneRenderPass{ pass, context, graph, device, category, name, renderPassDesc }
		, m_scene{ renderPassDesc.m_culler.getScene() }
		, m_camera{ renderPassDesc.m_culler.hasCamera() ? &renderPassDesc.m_culler.getCamera() : nullptr }
		, m_environment{ techniquePassDesc.m_environment }
		, m_hasVelocity{ techniquePassDesc.m_hasVelocity }
		, m_ssaoConfig{ techniquePassDesc.m_ssaoConfig }
		, m_ssao{ techniquePassDesc.m_ssao }
		, m_lpvConfigUbo{ techniquePassDesc.m_lpvConfigUbo }
		, m_llpvConfigUbo{ techniquePassDesc.m_llpvConfigUbo }
		, m_vctConfigUbo{ techniquePassDesc.m_vctConfigUbo }
		, m_lpvResult{ techniquePassDesc.m_lpvResult }
		, m_llpvResult{ techniquePassDesc.m_llpvResult }
		, m_vctFirstBounce{ techniquePassDesc.m_vctFirstBounce }
		, m_vctSecondaryBounce{ techniquePassDesc.m_vctSecondaryBounce }
		, m_finished{ m_device->createSemaphore( name ) }
	{
	}

	void RenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
	}

	void RenderTechniquePass::update( GpuUpdater & updater )
	{
		doUpdateNodes( m_renderQueue.getCulledRenderNodes()
			, updater.jitter
			, updater.info );
	}

	void RenderTechniquePass::doUpdateNodes( QueueCulledRenderNodes & nodes
		, castor::Point2f const & jitter
		, RenderInfo & info )
	{
		if ( nodes.hasNodes() )
		{
			SceneRenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.staticNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.morphingNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.billboardNodes.frontCulled );

			SceneRenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, info );
			SceneRenderPass::doUpdate( nodes.staticNodes.backCulled, info );
			SceneRenderPass::doUpdate( nodes.skinnedNodes.backCulled, info );
			SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, info );
			SceneRenderPass::doUpdate( nodes.morphingNodes.backCulled, info );
			SceneRenderPass::doUpdate( nodes.billboardNodes.backCulled, info );
		}
	}

	void RenderTechniquePass::doUpdateUbos( CpuUpdater & updater )
	{
		m_sceneUbo.cpuUpdate( *updater.camera->getScene()
			, updater.camera.get() );
	}

	void RenderTechniquePass::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eLighting );

		if ( m_environment )
		{
			addFlag( flags.programFlags, ProgramFlag::eEnvironmentMapping );
		}

		if ( !m_vctConfigUbo || !m_vctFirstBounce || !m_vctSecondaryBounce )
		{
			remFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing );
		}

		if ( !m_lpvConfigUbo || !m_lpvResult )
		{
			remFlag( flags.sceneFlags, SceneFlag::eLpvGI );
		}

		if ( !m_llpvConfigUbo || !m_lpvResult )
		{
			remFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );
		}
	}

	ShaderPtr RenderTechniquePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	void RenderTechniquePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
		m_sceneUbo.cpuUpdate( m_scene, m_camera );
	}

	void RenderTechniquePass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( PassUboIdx::eCount );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) ); // c3d_mapOcclusion

		// Shadow maps
		for ( uint32_t j = 0u; j < uint32_t( LightType::eCount ); ++j )
		{
			if ( checkFlag( flags.sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << j ) ) )
			{
				// Depth
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				// Variance
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}
		}

		if ( checkFlag( flags.passFlags, PassFlag::eReflection )
			|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( checkFlag( flags.passFlags, PassFlag::eImageBasedLighting ) )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapIrradiance
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapPrefiltered
		}

		if ( checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( m_vctConfigUbo );
			CU_Require( m_vctFirstBounce );
			CU_Require( m_vctSecondaryBounce );
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
			if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvConfigUbo );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvConfigUbo );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvResult );
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

			if ( checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvResult );

				for ( auto & lpv : *m_llpvResult )
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

	namespace
	{
		void fillAdditionalDescriptor( crg::RunnableGraph & graph
			, RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, Scene const & scene
			, SceneNode const & sceneNode
			, ShadowMapLightTypeArray const & shadowMaps
			, Texture const * ssao
			, LpvGridConfigUbo const * lpvConfigUbo
			, LayeredLpvGridConfigUbo const * llpvConfigUbo
			, VoxelizerUbo const * vctConfigUbo
			, LightVolumePassResult const * lpvResult
			, LightVolumePassResultArray const * llpvResult
			, Texture const * vctFirstBounce
			, Texture const * vctSecondaryBounce )
		{
			auto index = uint32_t( PassUboIdx::eCount );
			auto & flags = pipeline.getFlags();
			descriptorWrites.push_back( scene.getLightCache().getDescriptorWrite( index++ ) );

			if ( ssao )
			{
				bindTexture( ssao->wholeView
					, *ssao->sampler
					, descriptorWrites
					, index );
			}

			bindShadowMaps( graph
				, pipeline.getFlags()
				, shadowMaps
				, descriptorWrites
				, index );

			if ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
			{
				auto & envMap = scene.getEnvironmentMap();
				auto envMapIndex = scene.getEnvironmentMapIndex( sceneNode );
				bindTexture( envMap.getColourView( envMapIndex )
					, *envMap.getColourId().sampler
					, descriptorWrites
					, index );
			}

			if ( checkFlag( flags.passFlags, PassFlag::eImageBasedLighting ) )
			{
				auto & background = *scene.getBackground();

				if ( background.hasIbl() )
				{
					auto & ibl = background.getIbl();
					bindTexture( ibl.getPrefilteredBrdfTexture().wholeView
						, ibl.getPrefilteredBrdfSampler()
						, descriptorWrites
						, index );
					bindTexture( ibl.getIrradianceTexture().wholeView
						, ibl.getIrradianceSampler()
						, descriptorWrites
						, index );
					bindTexture( ibl.getPrefilteredEnvironmentTexture().wholeView
						, ibl.getPrefilteredEnvironmentSampler()
						, descriptorWrites
						, index );
				}
			}

			if ( checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				CU_Require( vctConfigUbo );
				CU_Require( vctFirstBounce );
				CU_Require( vctSecondaryBounce );
				descriptorWrites.push_back( vctConfigUbo->getDescriptorWrite( index++ ) );
				bindTexture( vctFirstBounce->wholeView
					, *vctFirstBounce->sampler
					, descriptorWrites
					, index );
				bindTexture( vctSecondaryBounce->wholeView
					, *vctSecondaryBounce->sampler
					, descriptorWrites
					, index );
			}
			else
			{
				if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) )
				{
					CU_Require( lpvConfigUbo );
					descriptorWrites.push_back( lpvConfigUbo->getDescriptorWrite( index++ ) );
				}

				if ( checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					CU_Require( llpvConfigUbo );
					descriptorWrites.push_back( llpvConfigUbo->getDescriptorWrite( index++ ) );
				}

				if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) )
				{
					CU_Require( lpvResult );
					auto & lpv = *lpvResult;
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

				if ( checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					CU_Require( llpvResult );

					for ( auto & plpv : *llpvResult )
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
	}

	void RenderTechniquePass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( m_graph
			, pipeline
			, descriptorWrites
			, m_scene
			, node.sceneNode
			, shadowMaps
			, m_ssao
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_vctConfigUbo
			, m_lpvResult
			, m_llpvResult
			, m_vctFirstBounce
			, m_vctSecondaryBounce );
	}

	void RenderTechniquePass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( m_graph
			, pipeline
			, descriptorWrites
			, m_scene
			, node.sceneNode
			, shadowMaps
			, m_ssao
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_vctConfigUbo
			, m_lpvResult
			, m_llpvResult
			, m_vctFirstBounce
			, m_vctSecondaryBounce );
	}

	ashes::PipelineDepthStencilStateCreateInfo RenderTechniquePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		if ( m_environment )
		{
			return ashes::PipelineDepthStencilStateCreateInfo{ 0u
				, VK_TRUE
				, m_mode != RenderMode::eTransparentOnly };
		}
		else
		{
			return ashes::PipelineDepthStencilStateCreateInfo{ 0u
				, VK_TRUE
				, VK_FALSE
				, ( m_mode != RenderMode::eTransparentOnly
					? VK_COMPARE_OP_EQUAL
					: VK_COMPARE_OP_LESS_OR_EQUAL ) };
		}
	}

	ashes::PipelineColorBlendStateCreateInfo RenderTechniquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}

	ShaderPtr RenderTechniquePass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2
			&& "Can't have both instantiation and morphing yet." );
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		shader::VertexSurface inSurface{ writer
			, flags.programFlags
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( NodeUboIdx::eSkinningUbo )
			, uint32_t( NodeUboIdx::eSkinningSsbo )
			, RenderPipeline::eBuffers
			, flags.programFlags );
		UBO_MORPHING( writer
			, uint32_t( NodeUboIdx::eMorphing )
			, RenderPipeline::eBuffers
			, flags.programFlags );

		UBO_MATRIX( writer
			, uint32_t( PassUboIdx::eMatrix )
			, RenderPipeline::eAdditional );
		UBO_SCENE( writer
			, uint32_t( PassUboIdx::eScene )
			, RenderPipeline::eAdditional );

		// Outputs
		shader::OutFragmentSurface outSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto out = writer.getOut();

		writer.implementMain( [&]()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, inSurface.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( inSurface.normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( inSurface.tangent, 0.0_f ) );
				outSurface.texture = inSurface.texture;
				inSurface.morph( c3d_morphingData
					, curPosition
					, v4Normal
					, v4Tangent
					, outSurface.texture );
				outSurface.material = c3d_modelData.getMaterialIndex( flags.programFlags
					, inSurface.material );
				outSurface.instance = writer.cast< UInt >( in.instanceIndex );

				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
					, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, inSurface ) );
				auto prvMtxModel = writer.declLocale< Mat4 >( "prvMtxModel"
					, c3d_modelData.getPrvModelMtx( flags.programFlags, curMtxModel ) );
				auto prvPosition = writer.declLocale( "prvPosition"
					, prvMtxModel * curPosition );
				curPosition = curMtxModel * curPosition;
				outSurface.worldPosition = curPosition.xyz();
				outSurface.viewPosition = c3d_matrixData.worldToCurView( vec4( outSurface.worldPosition, 1.0f ) ).xyz();
				outSurface.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

				auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
					, c3d_modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
				outSurface.computeTangentSpace( flags.programFlags
					, c3d_sceneData.getCameraPosition()
					, mtxNormal
					, v4Normal
					, v4Tangent );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
