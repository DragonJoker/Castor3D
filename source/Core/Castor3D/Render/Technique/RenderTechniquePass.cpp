#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Node/RenderNode_Render.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
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
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"
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

	namespace
	{
		template< typename MapType >
		void doSortAlpha( MapType & input
			, Camera const & camera
			, SceneRenderPass::DistanceSortedNodeMap & output )
		{
			for ( auto & itPipelines : input )
			{
				for ( auto & renderNode : itPipelines.second )
				{
					Matrix4x4f worldMeshMatrix = renderNode.sceneNode.getDerivedTransformationMatrix().getInverse().transpose();
					Point3f worldCameraPosition = camera.getParent()->getDerivedPosition();
					Point3f meshCameraPosition = worldCameraPosition * worldMeshMatrix;
					renderNode.data.sortByDistance( meshCameraPosition );
					meshCameraPosition -= renderNode.sceneNode.getPosition();
					output.emplace( point::lengthSquared( meshCameraPosition ), makeDistanceNode( renderNode ) );
				}
			}
		}
	}

	//*************************************************************************************************

	void bindTexture( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		writes.push_back( ashes::WriteDescriptorSet
			{
				index++,
				0u,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				{ {
					sampler,
					view,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				} }
			} );
	}

	void bindShadowMaps( PipelineFlags const & pipelineFlags
		, ShadowMapLightTypeArray const & shadowMaps
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		for ( auto i = 0u; i < uint32_t( LightType::eCount ); ++i )
		{
			if ( checkFlag( pipelineFlags.sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << i ) ) )
			{
				for ( auto & shadowMapRef : shadowMaps[i] )
				{
					auto & shadowMap = shadowMapRef.first.get();
					auto & result = shadowMap.getShadowPassResult();
					CU_Require( result[SmTexture::eNormalLinear].getTexture()->isInitialised() );
					bindTexture( result[SmTexture::eNormalLinear].getTexture()->getDefaultView().getSampledView()
						, result[SmTexture::eNormalLinear].getSampler()->getSampler()
						, writes
						, index );
					CU_Require( result[SmTexture::eVariance].getTexture()->isInitialised() );
					bindTexture( result[SmTexture::eVariance].getTexture()->getDefaultView().getSampledView()
						, result[SmTexture::eVariance].getSampler()->getSampler()
						, writes
						, index );
				}
			}
		}
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( RenderDevice const & device
		, String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo
		, VoxelizerUbo const * vctConfigUbo )
		: SceneRenderPass{ device, category, name, matrixUbo, culler, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
		, m_vctConfigUbo{ vctConfigUbo }
	{
	}

	RenderTechniquePass::RenderTechniquePass( RenderDevice const & device
		, String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo
		, VoxelizerUbo const * vctConfigUbo )
		: SceneRenderPass{ device, category, name, matrixUbo, culler, oit, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
		, m_vctConfigUbo{ vctConfigUbo }
	{
	}

	RenderTechniquePass::RenderTechniquePass( RenderDevice const & device
		, castor::Size const & size
		, String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo
		, VoxelizerUbo const * vctConfigUbo
		, LightVolumePassResult const * lpvResult
		, TextureUnit const * vctFirstBounce
		, TextureUnit const * vctSecondaryBounce )
		: SceneRenderPass{ device, category, name, matrixUbo, culler, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
		, m_vctConfigUbo{ vctConfigUbo }
	{
		initialise( size
			, lpvResult
			, vctFirstBounce
			, vctSecondaryBounce );
	}

	RenderTechniquePass::RenderTechniquePass( RenderDevice const & device
		, castor::Size const & size
		, String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo
		, VoxelizerUbo const * vctConfigUbo
		, LightVolumePassResult const * lpvResult
		, TextureUnit const * vctFirstBounce
		, TextureUnit const * vctSecondaryBounce )
		: SceneRenderPass{ device, category, name, matrixUbo, culler, oit, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
		, m_vctConfigUbo{ vctConfigUbo }
	{
		initialise( size
			, lpvResult
			, vctFirstBounce
			, vctSecondaryBounce );
	}

	bool RenderTechniquePass::initialise( castor::Size const & size
		, LightVolumePassResult const * lpvResult
		, TextureUnit const * vctFirstBounce
		, TextureUnit const * vctSecondaryBounce )
	{
		m_lpvResult = lpvResult;
		m_vctFirstBounce = vctFirstBounce;
		m_vctSecondaryBounce = vctSecondaryBounce;
		return SceneRenderPass::initialise( size );
	}

	bool RenderTechniquePass::initialise( castor::Size const & size
		, RenderPassTimer & timer
		, uint32_t index
		, LightVolumePassResult const * lpvResult
		, TextureUnit const * vctFirstBounce
		, TextureUnit const * vctSecondaryBounce )
	{
		m_lpvResult = lpvResult;
		m_vctFirstBounce = vctFirstBounce;
		m_vctSecondaryBounce = vctSecondaryBounce;
		return SceneRenderPass::initialise( size, timer, index );
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

	void RenderTechniquePass::doUpdateNodes( SceneCulledRenderNodes & nodes
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

	bool RenderTechniquePass::doInitialise( Size const & CU_UnusedParam( size ) )
	{
		m_finished = m_device->createSemaphore( getName() );
		return true;
	}

	void RenderTechniquePass::doCleanup()
	{
		m_renderQueue.cleanup();
		m_finished.reset();
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
				, VK_COMPARE_OP_EQUAL };
		}
	}

	ashes::PipelineColorBlendStateCreateInfo RenderTechniquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}

	void RenderTechniquePass::doFillUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		auto sceneFlags = pipeline.getFlags().sceneFlags;

		if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( m_vctConfigUbo );
			m_vctConfigUbo->createSizedBinding( *node.uboDescriptorSet
				, layout.getBinding( uint32_t( NodeUboIdx::eVoxelData ) ) );
		}
		else
		{
			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvConfigUbo );
				m_lpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eLpvGridConfig ) ) );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvConfigUbo );
				m_llpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eLayeredLpvGridConfig ) ) );
			}
		}
	}

	void RenderTechniquePass::doFillUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		auto sceneFlags = pipeline.getFlags().sceneFlags;

		if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( m_vctConfigUbo );
			m_vctConfigUbo->createSizedBinding( *node.uboDescriptorSet
				, layout.getBinding( uint32_t( NodeUboIdx::eVoxelData ) ) );
		}
		else
		{
			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvConfigUbo );
				m_lpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eLpvGridConfig ) ) );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvConfigUbo );
				m_llpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eLayeredLpvGridConfig ) ) );
			}
		}
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderTechniquePass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = SceneRenderPass::doCreateUboBindings( flags );

		if ( checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( m_vctConfigUbo );
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eVoxelData )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}
		else
		{
			if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvConfigUbo );
				uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eLpvGridConfig )
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvConfigUbo );
				uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eLayeredLpvGridConfig )
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}
		}

		return uboBindings;
	}

	namespace
	{
		template< typename DataTypeT, typename InstanceTypeT >
		void fillTexDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ObjectRenderNode< DataTypeT, InstanceTypeT > & node
			, ShadowMapLightTypeArray const & shadowMaps
			, Scene const & scene
			, LightVolumePassResult const * lpvResult
			, TextureUnit const * vctFirstBounce
			, TextureUnit const * vctSecondaryBounce )
		{
			auto & flags = pipeline.getFlags();
			ashes::WriteDescriptorSetArray writes;

			if ( !flags.textures.empty() )
			{
				node.passNode.fillDescriptor( layout
					, index
					, writes
					, flags.textures );
			}

			if ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
			{
				auto & envMap = scene.getEnvironmentMap( node.sceneNode );
				bindTexture( envMap.getTexture().getTexture()->getDefaultView().getSampledView()
					, envMap.getTexture().getSampler()->getSampler()
					, writes
					, index );
			}

			if ( checkFlag( flags.passFlags, PassFlag::eMetallicRoughness )
				|| checkFlag( flags.passFlags, PassFlag::eSpecularGlossiness ) )
			{
				auto & background = *node.sceneNode.getScene()->getBackground();

				if ( background.hasIbl() )
				{
					auto & ibl = background.getIbl();
					bindTexture( ibl.getIrradianceTexture()
						, ibl.getIrradianceSampler()
						, writes
						, index );
					bindTexture( ibl.getPrefilteredEnvironmentTexture()
						, ibl.getPrefilteredEnvironmentSampler()
						, writes
						, index );
					bindTexture( ibl.getPrefilteredBrdfTexture()
						, ibl.getPrefilteredBrdfSampler()
						, writes
						, index );
				}
			}

			if ( checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				CU_Require( vctFirstBounce );
				CU_Require( vctSecondaryBounce );
				bindTexture( vctFirstBounce->getTexture()->getDefaultView().getSampledView()
					, vctFirstBounce->getSampler()->getSampler()
					, writes
					, index );
				bindTexture( vctSecondaryBounce->getTexture()->getDefaultView().getSampledView()
					, vctSecondaryBounce->getSampler()->getSampler()
					, writes
					, index );
			}
			else
			{
				if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
					|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					CU_Require( lpvResult );
					auto & lpv = *lpvResult;
						bindTexture( lpv[LpvTexture::eR].getTexture()->getDefaultView().getSampledView()
							, lpv[LpvTexture::eR].getSampler()->getSampler()
							, writes
							, index );
					bindTexture( lpv[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
						, lpv[LpvTexture::eG].getSampler()->getSampler()
						, writes
						, index );
					bindTexture( lpv[LpvTexture::eG].getTexture()->getDefaultView().getSampledView()
						, lpv[LpvTexture::eG].getSampler()->getSampler()
						, writes
						, index );
				}
			}

			bindShadowMaps( pipeline.getFlags()
				, shadowMaps
				, writes
				, index );
			node.texDescriptorSet->setBindings( writes );
		}
	}

	void RenderTechniquePass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( pipeline
			, layout
			, index
			, node
			, shadowMaps
			, m_scene
			, m_lpvResult
			, m_vctFirstBounce
			, m_vctSecondaryBounce );
	}

	void RenderTechniquePass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( pipeline
			, layout
			, index
			, node
			, shadowMaps
			, m_scene
			, m_lpvResult
			, m_vctFirstBounce
			, m_vctSecondaryBounce );
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderTechniquePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = 0u;
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( !flags.textures.empty() )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( flags.textures.size() ) ) );
			index += uint32_t( flags.textures.size() );
		}

		if ( checkFlag( flags.passFlags, PassFlag::eReflection )
			|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( checkFlag( flags.passFlags, PassFlag::eMetallicRoughness )
			|| checkFlag( flags.passFlags, PassFlag::eSpecularGlossiness ) )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapIrradiance
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapPrefiltered
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf
		}

		if ( checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( m_vctFirstBounce );
			CU_Require( m_vctSecondaryBounce );
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapVoxelsFirstBounce
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapVoxelsSecondaryBounce
		}
		else
		{
			if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
				|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_lpvResult );
				textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationR
				textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationG
				textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_lpvAccumulationB
			}
		}

		for ( uint32_t j = 0u; j < uint32_t( LightType::eCount ); ++j )
		{
			// Depth
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			// Variance
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		return textureBindings;
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

		UBO_MATRIX( writer, uint32_t( NodeUboIdx::eMatrix ), 0 );
		UBO_SCENE( writer, uint32_t( NodeUboIdx::eScene ), 0 );
		UBO_MODEL( writer, uint32_t( NodeUboIdx::eModel ), 0 );
		auto skinningData = SkinningUbo::declare( writer, uint32_t( NodeUboIdx::eSkinning ), 0, flags.programFlags );
		UBO_MORPHING( writer, uint32_t( NodeUboIdx::eMorphing ), 0, flags.programFlags );

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
