#include "Castor3D/Render/Technique/Opaque/Lighting/SubsurfaceScatteringPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
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
	namespace
	{
		enum BlurId : size_t
		{
			BlurMaterialsUboId,
			BlurSceneUboId,
			BlurGpInfoUboId,
			BlurSssUboId,
			BlurData0ImgId,
			BlurData4ImgId,
			BlurLgtDiffImgId,
		};

		enum CombId : size_t
		{
			CombMaterialsUboId,
			CombData0ImgId,
			CombData4ImgId,
			CombBlur1ImgId,
			CombBlur2ImgId,
			CombBlur3ImgId,
			CombLgtDiffImgId,
		};

		ShaderPtr getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurProgram( RenderSystem const & renderSystem
			, bool isVertic )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer, PassFlag( 0u ) );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers(), BlurMaterialsUboId, 0u );
			UBO_GPINFO( writer, BlurGpInfoUboId, 0u );
			Ubo config{ writer, SubsurfaceScatteringPass::Config, BlurSssUboId, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( SubsurfaceScatteringPass::PixelSize );
			auto c3d_correction = config.declMember< Float >( SubsurfaceScatteringPass::Correction );
			config.end();
			auto c3d_mapData0 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData0 ), BlurData0ImgId, 0u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), BlurData4ImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", BlurLgtDiffImgId, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer, *renderSystem.getEngine() };
			utils.declareCalcVSPosition();
			utils.declareInvertVec2Y();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto data0 = writer.declLocale( "data0"
						, c3d_mapData0.lod( vtx_texture, 0.0_f ) );
					auto data4 = writer.declLocale( "data4"
						, c3d_mapData4.lod( vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data0.w() ) );
					auto translucency = writer.declLocale( "translucency"
						, data4.w() );
					auto material = materials->getMaterial( materialId );

					IF( writer, material.subsurfaceScatteringEnabled == 0_i )
					{
						writer.discard();
					}
					FI;

					// Fetch color and linear depth for current pixel:
					auto colorM = writer.declLocale( "colorM"
						, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ) );
					auto depthM = writer.declLocale( "depthM"
						, c3d_mapData0.lod( vtx_texture, 0.0_f ).x() );
					depthM = c3d_gpInfoData.projToView( utils, vtx_texture, depthM ).z();

					// Accumulate center sample, multiplying it with its gaussian weight:
					pxl_fragColor = colorM;
					pxl_fragColor.rgb() *= 0.382_f;

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
					auto finalStep = writer.declLocale( "finalStep"
						, translucency * step * material.subsurfaceScatteringStrength * material.gaussianWidth / depthM );

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
						depth = c3d_mapData0.lod( offset, 0.0_f ).x();
						depth = c3d_gpInfoData.projToView( utils, vtx_texture, depth ).z();

						// If the difference in depth is huge, we lerp color back to "colorM":
						s = min( 0.0125_f * c3d_correction * abs( depthM - depth ), 1.0_f );
						color = mix( color, colorM.rgb(), vec3( s ) );

						// Accumulate:
						pxl_fragColor.rgb() += w[i] * color;
					}
					ROF;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getCombineProgram( RenderSystem const & renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto materials = shader::createMaterials( writer, PassFlag( 0u ) );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers(), CombMaterialsUboId, 0u );

			auto c3d_mapData0 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData0 ), CombData0ImgId, 0u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), CombData4ImgId, 0u );
			auto c3d_mapBlur1 = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBlur1", CombBlur1ImgId, 0u );
			auto c3d_mapBlur2 = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBlur2", CombBlur2ImgId, 0u );
			auto c3d_mapBlur3 = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBlur3", CombBlur3ImgId, 0u );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", CombLgtDiffImgId, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			shader::Utils utils{ writer, *renderSystem.getEngine() };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto data0 = writer.declLocale( "data0"
						, c3d_mapData0.lod( vtx_texture, 0.0_f ) );
					auto original = writer.declLocale( "original"
						, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data0.w() ) );
					auto material = materials->getMaterial( materialId );

					IF( writer, material.subsurfaceScatteringEnabled == 0_i )
					{
						pxl_fragColor = original;
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
						auto data4 = writer.declLocale( "data4"
							, c3d_mapData4.lod( vtx_texture, 0.0_f ) );
						auto translucency = writer.declLocale( "translucency"
							, data4.w() );
						pxl_fragColor = original * originalWeight
							+ blur1 * blurWeights[0]
							+ blur2 * blurWeights[1]
							+ blur3 * blurWeights[2];
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		Texture doCreateImage( crg::FrameGraph & graph
			, RenderDevice const & device
			, castor::Size const & size
			, VkFormat format
			, std::string name )
		{
			return { device
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
				, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK };
		}

		crg::rq::Config createConfig( castor::Size const & size
			, ashes::PipelineShaderStageCreateInfoArray const & shaderStages
			, bool const * enabled )
		{
			crg::rq::Config result;
			result.texcoordConfig( crg::Texcoord{} );
			result.renderSize( makeExtent2D( size ) );
			result.programs( { crg::makeVkArray< VkPipelineShaderStageCreateInfo >( shaderStages ) } );
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

	SubsurfaceScatteringPass::SubsurfaceScatteringPass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, ProgressBar * progress
		, Scene const & scene
		, GpInfoUbo const & gpInfoUbo
		, SceneUbo const & sceneUbo
		, castor::Size const & textureSize
		, OpaquePassResult const & gpResult
		, LightPassResult const & lpResult )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sceneUbo{ sceneUbo }
		, m_gpResult{ gpResult }
		, m_lpResult{ lpResult }
		, m_scene{ scene }
		, m_enabled{ m_scene.needsSubsurfaceScattering() }
		, m_size{ textureSize }
		, m_intermediate{ doCreateImage( graph, m_device, textureSize, m_lpResult[LpTexture::eDiffuse].getFormat(), "SSSIntermediate" ) }
		, m_blurImages{ doCreateImage( graph, m_device, textureSize, m_intermediate.getFormat(), "SSSBlur0" )
			, doCreateImage( graph, m_device, textureSize, m_intermediate.getFormat(), "SSSBlur1" )
			, doCreateImage( graph, m_device, textureSize, m_intermediate.getFormat(), "SSSBlur2" ) }
		, m_result{ doCreateImage( graph, m_device, textureSize, m_intermediate.getFormat(), "SSSResult" ) }
		, m_blurCfgUbo{ m_device.uboPools->getBuffer< BlurConfiguration >( 0u ) }
		, m_blurWgtUbo{ m_device.uboPools->getBuffer< BlurWeights >( 0u ) }
		, m_blurHorizVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SSSBlurX", getVertexProgram() }
		, m_blurHorizPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSSBlurX", getBlurProgram( m_device.renderSystem, false ) }
		, m_blurXShader{ makeShaderState( m_device, m_blurHorizVertexShader )
			, makeShaderState( m_device, m_blurHorizPixelShader ) }
		, m_blurVerticVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SSSBlurY", getVertexProgram() }
		, m_blurVerticPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSSBlurY", getBlurProgram( m_device.renderSystem, true ) }
		, m_blurYShader{ makeShaderState( m_device, m_blurVerticVertexShader )
			, makeShaderState( m_device, m_blurVerticPixelShader ) }
		, m_combineVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SSSCombine", getVertexProgram() }
		, m_combinePixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSSCombine", getCombineProgram( m_device.renderSystem ) }
		, m_combineShader{ makeShaderState( m_device, m_combineVertexShader )
			, makeShaderState( m_device, m_combinePixelShader ) }
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
		stepProgressBar( progress, "Creating SSS Blur passes" );

		for ( uint32_t i = 0u; i < PassCount; ++i )
		{
			auto blurYDestination = &m_blurImages[i];
			auto & blurX = graph.createPass( "SSSBlurX" + std::to_string( i )
				, [this]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::RenderQuad >( pass
						, context
						, graph
						, 1u
						, createConfig( m_size, m_blurXShader, &m_enabled ) );
					getEngine()->registerTimer( graph.getName() + "/SSS"
						, result->getTimer() );
					return result;
				} );
			blurX.addDependency( *previousPass );
			previousPass = &blurX;
			getEngine()->getMaterialCache().getPassBuffer().createPassBinding( blurX
				, BlurMaterialsUboId );
			m_sceneUbo.createPassBinding( blurX
				, BlurSceneUboId );
			m_gpInfoUbo.createPassBinding( blurX
				, BlurGpInfoUboId );
			m_blurCfgUbo.createPassBinding( blurX
				, "BlurCfg"
				, BlurSssUboId );
			blurX.addSampledView( m_gpResult[DsTexture::eData0].sampledViewId
				, BlurData0ImgId
				, VK_IMAGE_LAYOUT_UNDEFINED );
			blurX.addSampledView( m_gpResult[DsTexture::eData4].sampledViewId
				, BlurData4ImgId
				, VK_IMAGE_LAYOUT_UNDEFINED );
			blurX.addSampledView( blurXSource->sampledViewId
				, BlurLgtDiffImgId
				, VK_IMAGE_LAYOUT_UNDEFINED );
			blurX.addOutputColourView( m_intermediate.targetViewId );

			auto & blurY = graph.createPass( "SSSBlurY" + std::to_string( i )
				, [this]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::RenderQuad >( pass
						, context
						, graph
						, 1u
						, createConfig( m_size, m_blurYShader, &m_enabled ) );
					getEngine()->registerTimer( graph.getName() + "/SSS"
						, result->getTimer() );
					return result;
				} );
			blurY.addDependency( *previousPass );
			previousPass = &blurY;
			getEngine()->getMaterialCache().getPassBuffer().createPassBinding( blurY
				, BlurMaterialsUboId );
			m_sceneUbo.createPassBinding( blurY
				, BlurSceneUboId );
			m_gpInfoUbo.createPassBinding( blurY
				, BlurGpInfoUboId );
			m_blurCfgUbo.createPassBinding( blurY
				, "BlurCfg"
				, BlurSssUboId );
			blurY.addSampledView( m_gpResult[DsTexture::eData0].sampledViewId
				, BlurData0ImgId
				, VK_IMAGE_LAYOUT_UNDEFINED );
			blurY.addSampledView( m_gpResult[DsTexture::eData4].sampledViewId
				, BlurData4ImgId
				, VK_IMAGE_LAYOUT_UNDEFINED );
			blurY.addSampledView( m_intermediate.sampledViewId
				, BlurLgtDiffImgId
				, VK_IMAGE_LAYOUT_UNDEFINED );
			blurY.addOutputColourView( blurYDestination->targetViewId );

			blurXSource = blurYDestination;
		}

		stepProgressBar( progress, "Creating SSS combine pass" );
		auto & pass = graph.createPass("SSSCombine"
			, [this, progress]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising SSS combine pass" );
				auto config = createConfig( m_size, m_combineShader, &m_enabled );
				config.recordDisabledInto( [this, &graph, &context]( crg::RunnablePass const & runnable
						, VkCommandBuffer commandBuffer
						, uint32_t passIndex )
					{
						auto & srcView = m_lpResult[LpTexture::eDiffuse].wholeViewId;
						auto & dstView = m_result.wholeViewId;
						auto size = m_result.getExtent();
						auto & srcSubresource = srcView.data->info.subresourceRange;
						auto & dstSubresource = dstView.data->info.subresourceRange;
						VkImageCopy region{ VkImageSubresourceLayers{ srcSubresource.aspectMask, srcSubresource.baseMipLevel, srcSubresource.baseArrayLayer, 1u }
							, VkOffset3D{ 0u, 0u, 0u }
							, VkImageSubresourceLayers{ dstSubresource.aspectMask, dstSubresource.baseMipLevel, dstSubresource.baseArrayLayer, 1u }
							, VkOffset3D{ 0u, 0u, 0u }
							, { size.width, size.height, 1u } };
						auto srcTransition = runnable.getTransition( passIndex, srcView );
						auto dstTransition = runnable.getTransition( passIndex, dstView );
						graph.memoryBarrier( commandBuffer
							, srcView
							, srcTransition.to
							, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) } );
						graph.memoryBarrier( commandBuffer
							, dstView
							, dstTransition.to
							, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) } );
						context.vkCmdCopyImage( commandBuffer
							, graph.createImage( srcView.data->image )
							, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
							, graph.createImage( dstView.data->image )
							, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
							, 1u
							, &region );
						graph.memoryBarrier( commandBuffer
							, dstView
							, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) }
							, dstTransition.to );
						graph.memoryBarrier( commandBuffer
							, srcView
							, { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
								, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
								, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) }
							, srcTransition.to );
					} );
				auto result = std::make_unique< crg::RenderQuad >( pass
					, context
					, graph
					, 1u
					, config );
				getEngine()->registerTimer( graph.getName() + "/SSS"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *previousPass );
		previousPass = &pass;
		getEngine()->getMaterialCache().getPassBuffer().createPassBinding( pass
			, CombMaterialsUboId );
		pass.addSampledView( m_gpResult[DsTexture::eData0].sampledViewId
			, CombData0ImgId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData4].sampledViewId
			, CombData4ImgId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_blurImages[0].sampledViewId
			, CombBlur1ImgId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_blurImages[1].sampledViewId
			, CombBlur2ImgId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_blurImages[2].sampledViewId
			, CombBlur3ImgId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_lpResult[LpTexture::eDiffuse].sampledViewId
			, CombLgtDiffImgId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addOutputColourView( m_result.targetViewId );

		m_result.create();
		m_intermediate.create();

		for ( auto & texture : m_blurImages )
		{
			texture.create();
		}
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
				, m_blurImages[i]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, TextureFactors{}.invert( true ) );
		}

		visitor.visit( "SSSSS Result"
			, m_result
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
