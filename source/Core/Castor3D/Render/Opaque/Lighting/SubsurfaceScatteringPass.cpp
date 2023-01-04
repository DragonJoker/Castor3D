#include "Castor3D/Render/Opaque/Lighting/SubsurfaceScatteringPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <random>

CU_ImplementCUSmartPtr( castor3d, SubsurfaceScatteringPass )

namespace castor3d
{
	namespace sssss
	{
		enum BlurId : size_t
		{
			BlurMaterialsUboId,
			BlurSssProfilesUboId,
			BlurModelsUboId,
			BlurSceneUboId,
			BlurGpInfoUboId,
			BlurSssUboId,
			BlurDepthObjImgId,
			BlurEmsTrnImgId,
			BlurLgtDiffImgId,
		};

		enum CombId : size_t
		{
			CombMaterialsUboId,
			CombModelsUboId,
			CombDepthObjImgId,
			CombEmsTrnImgId,
			CombBlur1ImgId,
			CombBlur2ImgId,
			CombBlur3ImgId,
			CombLgtDiffImgId,
		};

		static ShaderPtr getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getBlurProgram( Engine const & engine
			, bool isVertic )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::Utils utils{ writer };
			shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting
				, utils };
			shader::Materials materials{ writer, passShaders, BlurMaterialsUboId, 0u };
			shader::SssProfiles sssProfiles{ writer, BlurSssProfilesUboId, 0u };
			C3D_ModelsData( writer, BlurModelsUboId, 0u );
			C3D_GpInfo( writer, BlurGpInfoUboId, 0u );
			UniformBuffer config{ writer, SubsurfaceScatteringPass::Config, BlurSssUboId, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( SubsurfaceScatteringPass::PixelSize );
			auto c3d_correction = config.declMember< Float >( SubsurfaceScatteringPass::Correction );
			config.end();
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", BlurDepthObjImgId, 0u );
			auto c3d_mapEmsTrn = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eEmsTrn ), BlurEmsTrnImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightDiffuse", BlurLgtDiffImgId, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( vtx_texture, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					IF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					FI;

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto materialId = writer.declLocale( "materialId"
						, modelData.getMaterialId() );
					auto material = materials.getMaterial( materialId );
					auto sssProfileIndex = material.getMember< sdw::UInt >( "sssProfileIndex", true );

					IF( writer, sssProfileIndex == 0_u )
					{
						writer.demote();
					}
					FI;

					auto sssProfile = writer.declLocale( "sssProfile"
						, sssProfiles.getProfile( sssProfileIndex ) );

					// Fetch color and linear depth for current pixel:
					auto colorM = writer.declLocale( "colorM"
						, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ) );
					auto depthM = writer.declLocale( "depthM"
						, depthObj.x() );
					depthM = c3d_gpInfoData.projToView( utils, vtx_texture, depthM ).z();

					// Accumulate center sample, multiplying it with its gaussian weight:
					outColour = colorM;
					outColour.rgb() *= 0.382_f;

					if ( isVertic )
					{
						auto step = writer.declLocale( "step"
							, c3d_pixelSize * vec2( 0.0_f, 1.0_f ) );
					}
					else
					{
						auto step = writer.declLocale( "step"
							, c3d_pixelSize * vec2( 1.0_f, 0.0_f ) );
					}

					auto step = writer.getVariable< Vec2 >( "step" );

					// Calculate the step that we will use to fetch the surrounding pixels,
					// where "step" is:
					//     step = sssStrength * gaussianWidth * pixelSize * dir
					// The closer the pixel, the stronger the effect needs to be, hence
					// the factor 1.0 / depthM.
					auto emsTrn = writer.declLocale( "emsTrn"
						, c3d_mapEmsTrn.lod( vtx_texture, 0.0_f ) );
					auto translucency = writer.declLocale( "translucency"
						, emsTrn.w() );
					auto finalStep = writer.declLocale( "finalStep"
						, translucency * step * sssProfile.subsurfaceScatteringStrength() * sssProfile.gaussianWidth() / depthM );

					auto offset = writer.declLocale< Vec2 >( "offset" );
					auto color = writer.declLocale< Vec3 >( "color" );
					auto depth = writer.declLocale< Float >( "depth" );
					auto s = writer.declLocale< Float >( "s" );

					// Gaussian weights for the six samples around the current pixel:
					//   -3 -2 -1 +1 +2 +3
					auto w = writer.declLocaleArray( "w"
						, 6u
						, std::vector< Float >{ { 0.006_f, 0.061_f, 0.242_f, 0.242_f, 0.061_f, 0.006_f } } );
					auto o = writer.declLocaleArray( "o"
						, 6u
						, std::vector< Float >{ { -1.0_f, -0.666666667_f, -0.333333333_f, 0.333333333_f, 0.666666667_f, 1.0_f } } );

					// Accumulate the other samples:
					FOR( writer, Int, i, 0_i, i < 6_i, ++i )
					{
						// Fetch color and depth for current sample:
						offset = sdw::fma( vec2( o[i] ), finalStep, vtx_texture );
						color = c3d_mapLightDiffuse.lod( offset, 0.0_f ).rgb();
						offset = sdw::fma( vec2( o[i] ), finalStep, vtx_texture );
						depth = c3d_mapDepthObj.lod( offset, 0.0_f ).x();
						depth = c3d_gpInfoData.projToView( utils, vtx_texture, depth ).z();

						// If the difference in depth is huge, we lerp color back to "colorM":
						s = min( 0.0125_f * c3d_correction * abs( depthM - depth ), 1.0_f );
						color = mix( color, colorM.rgb(), vec3( s ) );

						// Accumulate:
						outColour.rgb() += w[i] * color;
					}
					ROF;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getCombineProgram( Engine const & engine )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::Utils utils{ writer };
			shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting
				, utils };
			shader::Materials materials{ writer, passShaders, CombMaterialsUboId, 0u };
			C3D_ModelsData( writer, CombModelsUboId, 0u );

			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", CombDepthObjImgId, 0u );
			auto c3d_mapEmsTrn = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eEmsTrn ), CombEmsTrnImgId, 0u );
			auto c3d_mapBlur1 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBlur1", CombBlur1ImgId, 0u );
			auto c3d_mapBlur2 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBlur2", CombBlur2ImgId, 0u );
			auto c3d_mapBlur3 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBlur3", CombBlur3ImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightDiffuse", CombLgtDiffImgId, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( vtx_texture, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					IF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					FI;

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto original = writer.declLocale( "original"
						, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, modelData.getMaterialId() );
					auto material = materials.getMaterial( materialId );
					auto sssProfileIndex = material.getMember< sdw::UInt >( "sssProfileIndex", true );

					IF( writer, sssProfileIndex == 0_u )
					{
						outColour = original;
					}
					ELSE
					{
						auto originalWeight = writer.declLocale< Vec4 >( "originalWeight"
							, vec4( 0.2406_f, 0.4475_f, 0.6159_f, 0.25_f ) );
						auto blurWeights = writer.declLocaleArray< Vec4 >( "blurWeights"
							, 3u
							, {
								vec4( 0.1158_f, 0.3661_f, 0.3439_f, 0.25_f ),
								vec4( 0.1836_f, 0.1864_f, 0.0_f, 0.25_f ),
								vec4( 0.46_f, 0.0_f, 0.0402_f, 0.25_f )
							} );
						auto blurVariances = writer.declLocaleArray< Float >( "blurVariances"
							, 3u
							, {
								0.0516_f,
								0.2719_f,
								2.0062_f
							} );
						auto blur1 = writer.declLocale( "blur1"
							, c3d_mapBlur1.lod( vtx_texture, 0.0_f ) );
						auto blur2 = writer.declLocale( "blur2"
							, c3d_mapBlur2.lod( vtx_texture, 0.0_f ) );
						auto blur3 = writer.declLocale( "blur3"
							, c3d_mapBlur3.lod( vtx_texture, 0.0_f ) );
						auto emsTrn = writer.declLocale( "emsTrn"
							, c3d_mapEmsTrn.lod( vtx_texture, 0.0_f ) );
						auto translucency = writer.declLocale( "translucency"
							, emsTrn.w() );
						outColour = original * originalWeight
							+ blur1 * blurWeights[0]
							+ blur2 * blurWeights[1]
							+ blur3 * blurWeights[2];
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static TexturePtr doCreateImage( crg::FramePassGroup & graph
			, RenderDevice const & device
			, castor::Size const & size
			, VkFormat format
			, std::string name )
		{
			return std::make_shared< Texture >( device
				, graph.getHandler()
				, name
				, 0u
				, makeExtent3D( size )
				, 1u
				, 1u
				, format
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
				, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK );
		}

		static crg::rq::Config createConfig( castor::Size const & size
			, ashes::PipelineShaderStageCreateInfoArray const & shaderStages
			, bool const * enabled )
		{
			crg::rq::Config result;
			result.texcoordConfig( crg::Texcoord{} );
			result.renderSize( makeExtent2D( size ) );
			result.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( shaderStages ) );
			result.enabled( enabled );
			return result;
		}
	}

	//*********************************************************************************************

	castor::String const SubsurfaceScatteringPass::Config = "Config";
	castor::String const SubsurfaceScatteringPass::Step = "c3d_step";
	castor::String const SubsurfaceScatteringPass::Correction = "c3d_correction";
	castor::String const SubsurfaceScatteringPass::PixelSize = "c3d_pixelSize";
	castor::String const SubsurfaceScatteringPass::Weights = "c3d_weights";
	castor::String const SubsurfaceScatteringPass::Offsets = "c3d_offsets";

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, ProgressBar * progress
		, Scene const & scene
		, GpInfoUbo const & gpInfoUbo
		, SceneUbo const & sceneUbo
		, Texture const & depthObj
		, OpaquePassResult const & gpResult
		, LightPassResult const & lpResult )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_gpResult{ gpResult }
		, m_lpResult{ lpResult }
		, m_scene{ scene }
		, m_group{ graph.createPassGroup( "SSSSS" ) }
		, m_enabled{ m_scene.needsSubsurfaceScattering() }
		, m_size{ makeSize( m_lpResult[LpTexture::eDiffuse].getExtent() ) }
		, m_intermediate{ sssss::doCreateImage( graph, m_device, m_size, m_lpResult[LpTexture::eDiffuse].getFormat(), "SSSIntermediate" ) }
		, m_blurImages{ sssss::doCreateImage( graph, m_device, m_size, m_intermediate->getFormat(), "SSSBlur0" )
			, sssss::doCreateImage( graph, m_device, m_size, m_intermediate->getFormat(), "SSSBlur1" )
			, sssss::doCreateImage( graph, m_device, m_size, m_intermediate->getFormat(), "SSSBlur2" ) }
		, m_result{ sssss::doCreateImage( graph, m_device, m_size, m_intermediate->getFormat(), "SSSResult" ) }
		, m_blurCfgUbo{ m_device.uboPool->getBuffer< BlurConfiguration >( 0u ) }
		, m_blurWgtUbo{ m_device.uboPool->getBuffer< BlurWeights >( 0u ) }
		, m_blurHorizVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SSSBlurX", sssss::getVertexProgram() }
		, m_blurHorizPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSSBlurX", sssss::getBlurProgram( *device.renderSystem.getEngine(), false ) }
		, m_blurXShader{ makeShaderState( m_device, m_blurHorizVertexShader )
			, makeShaderState( m_device, m_blurHorizPixelShader ) }
		, m_blurVerticVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SSSBlurY", sssss::getVertexProgram() }
		, m_blurVerticPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSSBlurY", sssss::getBlurProgram( *device.renderSystem.getEngine(), true ) }
		, m_blurYShader{ makeShaderState( m_device, m_blurVerticVertexShader )
			, makeShaderState( m_device, m_blurVerticPixelShader ) }
		, m_combineVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SSSCombine", sssss::getVertexProgram() }
		, m_combinePixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSSCombine", sssss::getCombineProgram( *device.renderSystem.getEngine() ) }
		, m_combineShader{ makeShaderState( m_device, m_combineVertexShader )
			, makeShaderState( m_device, m_combinePixelShader ) }
		, m_lastPass{ &previousPass }
	{
		auto & configuration = m_blurCfgUbo.getData();
		configuration.blurCorrection = 1.0f;
		configuration.blurPixelSize = castor::Point2f{ 1.0f / float( m_size.getWidth() ), 1.0f / float( m_size.getHeight() ) };

		auto & weights = m_blurWgtUbo.getData();
		weights.originalWeight = castor::Point4f{ 0.2406f, 0.4475f, 0.6159f, 0.25f };
		weights.blurWeights[0] = castor::Point4f{ 0.1158, 0.3661, 0.3439, 0.25 };
		weights.blurWeights[1] = castor::Point4f{ 0.1836, 0.1864, 0.0, 0.25 };
		weights.blurWeights[2] = castor::Point4f{ 0.46, 0.0, 0.0402, 0.25 };
		weights.blurVariance = castor::Point4f{ 0.0516, 0.2719, 2.0062 };
		auto blurXSource = &m_lpResult[LpTexture::eDiffuse];
		stepProgressBar( progress, "Creating SSSSS Blur passes" );
		auto & modelBuffer = scene.getModelBuffer().getBuffer();

		for ( uint32_t i = 0u; i < PassCount; ++i )
		{
			auto blurYDestination = m_blurImages[i].get();
			auto & blurX = m_group.createPass( "BlurX" + std::to_string( i )
				, [this]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::RenderQuad >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, sssss::createConfig( m_size, m_blurXShader, &m_enabled ) );
					getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			blurX.addDependency( *m_lastPass );
			m_lastPass = &blurX;
			getEngine()->getMaterialCache().getPassBuffer().createPassBinding( blurX
				, sssss::BlurMaterialsUboId );
			getEngine()->getMaterialCache().getSssProfileBuffer().createPassBinding( blurX
				, sssss::BlurSssProfilesUboId );
			blurX.addInputStorageBuffer( { modelBuffer, "Models" }
				, uint32_t( sssss::BlurModelsUboId )
				, 0u
				, uint32_t( modelBuffer.getSize() ) );
			m_sceneUbo.createPassBinding( blurX
				, sssss::BlurSceneUboId );
			m_gpInfoUbo.createPassBinding( blurX
				, sssss::BlurGpInfoUboId );
			m_blurCfgUbo.createPassBinding( blurX
				, "BlurCfg"
				, sssss::BlurSssUboId );
			blurX.addSampledView( depthObj.sampledViewId
				, sssss::BlurDepthObjImgId );
			blurX.addSampledView( m_gpResult[DsTexture::eEmsTrn].sampledViewId
				, sssss::BlurEmsTrnImgId );
			blurX.addSampledView( blurXSource->sampledViewId
				, sssss::BlurLgtDiffImgId );
			blurX.addOutputColourView( m_intermediate->targetViewId );

			auto & blurY = m_group.createPass( "BlurY" + std::to_string( i )
				, [this]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::RenderQuad >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, sssss::createConfig( m_size, m_blurYShader, &m_enabled ) );
					getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			blurY.addDependency( *m_lastPass );
			m_lastPass = &blurY;
			getEngine()->getMaterialCache().getPassBuffer().createPassBinding( blurY
				, sssss::BlurMaterialsUboId );
			getEngine()->getMaterialCache().getSssProfileBuffer().createPassBinding( blurY
				, sssss::BlurSssProfilesUboId );
			blurY.addInputStorageBuffer( { modelBuffer, "Models" }
				, uint32_t( sssss::BlurModelsUboId )
				, 0u
				, uint32_t( modelBuffer.getSize() ) );
			m_sceneUbo.createPassBinding( blurY
				, sssss::BlurSceneUboId );
			m_gpInfoUbo.createPassBinding( blurY
				, sssss::BlurGpInfoUboId );
			m_blurCfgUbo.createPassBinding( blurY
				, "BlurCfg"
				, sssss::BlurSssUboId );
			blurY.addSampledView( depthObj.sampledViewId
				, sssss::BlurDepthObjImgId );
			blurY.addSampledView( m_gpResult[DsTexture::eEmsTrn].sampledViewId
				, sssss::BlurEmsTrnImgId );
			blurY.addSampledView( m_intermediate->sampledViewId
				, sssss::BlurLgtDiffImgId );
			blurY.addOutputColourView( blurYDestination->targetViewId );

			blurXSource = blurYDestination;
		}

		stepProgressBar( progress, "Creating SSSSS combine pass" );
		auto & pass = m_group.createPass("Combine"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising SSSSS combine pass" );
				auto extent = m_result->getExtent();
				auto ruConfig = crg::ru::Config{}
					.implicitAction( m_result->wholeViewId
						, crg::RecordContext::copyImage( m_lpResult[LpTexture::eDiffuse].wholeViewId
							, m_result->wholeViewId
							, { extent.width, extent.height } ) );
				auto rqConfig = sssss::createConfig( m_size, m_combineShader, &m_enabled );
				auto result = std::make_unique< crg::RenderQuad >( framePass
					, context
					, graph
					, ruConfig
					, rqConfig );
				getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *m_lastPass );
		m_lastPass = &pass;
		getEngine()->getMaterialCache().getPassBuffer().createPassBinding( pass
			, sssss::CombMaterialsUboId );
		pass.addInputStorageBuffer( { modelBuffer, "Models" }
			, uint32_t( sssss::CombModelsUboId )
			, 0u
			, uint32_t( modelBuffer.getSize() ) );
		pass.addSampledView( depthObj.sampledViewId
			, sssss::CombDepthObjImgId );
		pass.addSampledView( m_gpResult[DsTexture::eEmsTrn].sampledViewId
			, sssss::CombEmsTrnImgId );
		pass.addSampledView( m_blurImages[0]->sampledViewId
			, sssss::CombBlur1ImgId );
		pass.addSampledView( m_blurImages[1]->sampledViewId
			, sssss::CombBlur2ImgId );
		pass.addSampledView( m_blurImages[2]->sampledViewId
			, sssss::CombBlur3ImgId );
		pass.addSampledView( m_lpResult[LpTexture::eDiffuse].sampledViewId
			, sssss::CombLgtDiffImgId );
		pass.addOutputColourView( m_result->targetViewId );

		m_result->create();
		m_intermediate->create();

		for ( auto & texture : m_blurImages )
		{
			texture->create();
		}
	}

	SubsurfaceScatteringPass::~SubsurfaceScatteringPass()
	{
		for ( auto & texture : m_blurImages )
		{
			texture->destroy();
		}

		m_intermediate->destroy();
		m_result->destroy();
	}

	void SubsurfaceScatteringPass::update( CpuUpdater & updater )
	{
		m_enabled = m_scene.needsSubsurfaceScattering();
	}

	void SubsurfaceScatteringPass::accept( PipelineVisitorBase & visitor )
	{
		for ( size_t i{ 0u }; i < m_blurImages.size(); ++i )
		{
			visitor.visit( "SSSSS Blur " + castor::string::toString( i )
				, *m_blurImages[i]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, TextureFactors{}.invert( true ) );
		}

		visitor.visit( "SSSSS Result"
			, *m_result
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		visitor.visit( m_blurHorizVertexShader );
		visitor.visit( m_blurHorizPixelShader );

		visitor.visit( m_blurVerticVertexShader );
		visitor.visit( m_blurVerticPixelShader );

		visitor.visit( m_combineVertexShader );
		visitor.visit( m_combinePixelShader );
	}
}
