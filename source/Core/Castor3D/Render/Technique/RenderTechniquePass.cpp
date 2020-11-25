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
		auto inPosition = writer.declInput< Vec4 >( "inPosition"
			, RenderPass::VertexInputs::PositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, RenderPass::VertexInputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, RenderPass::VertexInputs::TangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexInputs::TextureLocation
			, hasTextures );
		auto inBoneIds0 = writer.declInput< IVec4 >( "inBoneIds0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inBoneIds1 = writer.declInput< IVec4 >( "inBoneIds1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights0 = writer.declInput< Vec4 >( "inWeights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights1 = writer.declInput< Vec4 >( "inWeights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inTransform = writer.declInput< Mat4 >( "inTransform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inMaterial = writer.declInput< Int >( "inMaterial"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inPosition2 = writer.declInput< Vec4 >( "inPosition2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inNormal2 = writer.declInput< Vec3 >( "inNormal2"
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTangent2 = writer.declInput< Vec3 >( "inTangent2"
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTexture2 = writer.declInput< Vec3 >( "inTexture2"
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) && hasTextures );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		// Outputs
		auto outWorldPosition = writer.declOutput< Vec3 >( "outWorldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto outViewPosition = writer.declOutput< Vec3 >( "outViewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation
			, checkFlag( flags.programFlags, ProgramFlag::eLighting ) );
		auto outCurPosition = writer.declOutput< Vec3 >( "outCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto outPrvPosition = writer.declOutput< Vec3 >( "outPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto outTangentSpaceFragPosition = writer.declOutput< Vec3 >( "outTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto outTangentSpaceViewPosition = writer.declOutput< Vec3 >( "outTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto outNormal = writer.declOutput< Vec3 >( "outNormal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto outTangent = writer.declOutput< Vec3 >( "outTangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto outBitangent = writer.declOutput< Vec3 >( "outBitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture"
			, RenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto outInstance = writer.declOutput< UInt >( "outInstance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto outMaterial = writer.declOutput< UInt >( "outMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		writer.implementMain( [&]()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, vec4( inPosition.xyz(), 1.0_f ) );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( inNormal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( inTangent, 0.0_f ) );
				outTexture = inTexture;

				if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
				{
					auto curMtxModel = writer.declLocale( "curMtxModel"
						, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, curMtxModel );
				}
				else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					auto curMtxModel = writer.declLocale( "curMtxModel"
						, inTransform );
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, curMtxModel );
				}
				else
				{
					auto curMtxModel = writer.declLocale( "curMtxModel"
						, c3d_curMtxModel );
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, c3d_prvMtxModel );
				}

				auto curMtxModel = writer.getVariable< Mat4 >( "curMtxModel" );
				auto prvMtxModel = writer.getVariable< Mat4 >( "prvMtxModel" );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, transpose( inverse( mat3( curMtxModel ) ) ) );

				if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					outMaterial = writer.cast< UInt >( inMaterial );
				}
				else
				{
					outMaterial = writer.cast< UInt >( c3d_materialIndex );
				}

				if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
				{
					curPosition = vec4( sdw::mix( curPosition.xyz(), inPosition2.xyz(), vec3( c3d_time ) ), 1.0_f );
					v4Normal = vec4( sdw::mix( v4Normal.xyz(), inNormal2.xyz(), vec3( c3d_time ) ), 1.0_f );
					v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), inTangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
					outTexture = sdw::mix( outTexture, inTexture2, vec3( c3d_time ) );
				}

				auto prvPosition = writer.declLocale( "prvPosition"
					, prvMtxModel * curPosition );
				curPosition = curMtxModel * curPosition;
				outWorldPosition = curPosition.xyz();
				prvPosition = c3d_prvViewProj * prvPosition;
				curPosition = c3d_curViewProj * curPosition;
				outViewPosition = ( c3d_curView * vec4( outWorldPosition, 1.0f ) ).xyz();

				outNormal = normalize( mtxNormal * v4Normal.xyz() );
				outTangent = normalize( mtxNormal * v4Tangent.xyz() );
				outTangent = normalize( sdw::fma( -outNormal, vec3( dot( outTangent, outNormal ) ), outTangent ) );
				outBitangent = cross( outNormal, outTangent );

				if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
				{
					outNormal = -outNormal;
					outTangent = -outTangent;
					outBitangent = -outBitangent;
				}

				outInstance = writer.cast< UInt >( in.instanceIndex );

				auto tbn = writer.declLocale( "tbn"
					, transpose( mat3( outTangent, outBitangent, outNormal ) ) );
				outTangentSpaceFragPosition = tbn * outWorldPosition;
				outTangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();

				// Convert the jitter from non-homogeneous coordinates to homogeneous
				// coordinates and add it:
				// (note that for providing the jitter in non-homogeneous projection space,
				//  pixel coordinates (screen space) need to multiplied by two in the C++
				//  code)
				curPosition.xy() -= c3d_jitter * curPosition.w();
				prvPosition.xy() -= c3d_jitter * prvPosition.w();
				out.vtx.position = curPosition;

				outCurPosition = curPosition.xyw();
				outPrvPosition = prvPosition.xyw();
				// Positions in projection space are in [-1, 1] range, while texture
				// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
				// the scale (and flip the y axis):
				outCurPosition.xy() *= vec2( 0.5_f, -0.5_f );
				outPrvPosition.xy() *= vec2( 0.5_f, -0.5_f );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
