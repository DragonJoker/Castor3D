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
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

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
			, RenderPass::DistanceSortedNodeMap & output )
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

	RenderTechniquePass::RenderTechniquePass( String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo )
		: RenderPass{ category, name, *culler.getScene().getEngine(), matrixUbo, culler, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
	{
	}

	RenderTechniquePass::RenderTechniquePass( String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo )
		: RenderPass{ category, name, *culler.getScene().getEngine(), matrixUbo, culler, oit, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
	{
	}

	RenderTechniquePass::~RenderTechniquePass()
	{
	}

	bool RenderTechniquePass::initialise( RenderDevice const & device
		, castor::Size const & size
		, LightVolumePassResult const * lpvResult )
	{
		m_lpvResult = lpvResult;
		return RenderPass::initialise( device, size );
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
			RenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			RenderPass::doUpdate( nodes.staticNodes.frontCulled );
			RenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.morphingNodes.frontCulled );
			RenderPass::doUpdate( nodes.billboardNodes.frontCulled );

			RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.staticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.skinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.morphingNodes.backCulled, info );
			RenderPass::doUpdate( nodes.billboardNodes.backCulled, info );
		}
	}

	void RenderTechniquePass::doUpdateUbos( CpuUpdater & updater )
	{
		m_sceneUbo.cpuUpdate( *updater.camera->getScene()
			, updater.camera.get() );
	}

	bool RenderTechniquePass::doInitialise( RenderDevice const & device
		, Size const & CU_UnusedParam( size ) )
	{
		m_finished = device->createSemaphore( getName() );
		return true;
	}

	void RenderTechniquePass::doCleanup( RenderDevice const & device )
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
#if C3D_UseDepthPrepass
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_FALSE
			, VK_COMPARE_OP_EQUAL };
#else
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, m_mode != RenderMode::eTransparentOnly };
#endif
	}

	ashes::PipelineColorBlendStateCreateInfo RenderTechniquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}

	void RenderTechniquePass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		auto sceneFlags = node.pipeline.getFlags().sceneFlags;

		if ( checkFlag( sceneFlags, SceneFlag::eLpvGI )
			&& m_lpvConfigUbo )
		{
			m_lpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
				, layout.getBinding( LpvGridConfigUbo::BindingPoint ) );
		}

		if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI )
			&& m_llpvConfigUbo )
		{
			m_llpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
				, layout.getBinding( LayeredLpvGridConfigUbo::BindingPoint ) );
		}
	}

	void RenderTechniquePass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		auto sceneFlags = node.pipeline.getFlags().sceneFlags;

		if ( checkFlag( sceneFlags, SceneFlag::eLpvGI )
			&& m_lpvConfigUbo )
		{
			m_lpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
				, layout.getBinding( LpvGridConfigUbo::BindingPoint ) );
		}

		if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI )
			&& m_llpvConfigUbo )
		{
			m_llpvConfigUbo->createSizedBinding( *node.uboDescriptorSet
				, layout.getBinding( LayeredLpvGridConfigUbo::BindingPoint ) );
		}
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderTechniquePass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = RenderPass::doCreateUboBindings( flags );

		if ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
			&& m_lpvConfigUbo )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( LpvGridConfigUbo::BindingPoint//12
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI )
			&& m_llpvConfigUbo )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( LayeredLpvGridConfigUbo::BindingPoint//13
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		return uboBindings;
	}

	namespace
	{
		template< typename DataTypeT, typename InstanceTypeT >
		void fillTexDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ObjectRenderNode< DataTypeT, InstanceTypeT > & node
			, ShadowMapLightTypeArray const & shadowMaps
			, Scene const & scene
			, LightVolumePassResult const * lpvResult )
		{
			auto & flags = node.pipeline.getFlags();
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

			if ( lpvResult
				&& ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
					|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) ) )
			{
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

			bindShadowMaps( node.pipeline.getFlags()
				, shadowMaps
				, writes
				, index );
			node.texDescriptorSet->setBindings( writes );
		}
	}

	void RenderTechniquePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( layout
			, index
			, node
			, shadowMaps
			, m_scene
			, m_lpvResult );
	}

	void RenderTechniquePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( layout
			, index
			, node
			, shadowMaps
			, m_scene
			, m_lpvResult );
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderTechniquePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = getMinTextureIndex();
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

		if ( m_lpvResult
			&& ( checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
				|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) ) )
		{
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
}
