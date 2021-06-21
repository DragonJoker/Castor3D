#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

CU_ImplementCUSmartPtr( castor3d, WeightedBlendRendering )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		enum TransparentResolveIdx
		{
			SceneUboIndex,
			GpuInfoUboIndex,
			HdrUboIndex,
			DepthTexIndex,
			AccumTexIndex,
			RevealTexIndex,
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

		ShaderPtr getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_SCENE( writer, uint32_t( SceneUboIndex ), 0u );
			UBO_GPINFO( writer, uint32_t( GpuInfoUboIndex ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( HdrUboIndex ), 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( WbTexture::eDepth ), uint32_t( DepthTexIndex ), 0u );
			auto c3d_mapAccumulation = writer.declSampledImage< FImg2DRgba32 >( getTextureName( WbTexture::eAccumulation ), uint32_t( AccumTexIndex ), 0u );
			auto c3d_mapRevealage = writer.declSampledImage< FImg2DRgba32 >( getTextureName( WbTexture::eRevealage ), uint32_t( RevealTexIndex ), 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto in = writer.getIn();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			shader::Utils utils{ writer };
			utils.declareInvertVec2Y();
			utils.declareRemoveGamma();
			utils.declareCalcVSPosition();

			shader::CommonFog fog{ writer };

			auto maxComponent = writer.implementFunction< Float >( "maxComponent"
				, [&]( Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
			, InVec3{ writer, "v" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto coord = writer.declLocale( "coord"
						, ivec2( in.fragCoord.xy() ) );
					auto revealage = writer.declLocale( "revealage"
						, c3d_mapRevealage.fetch( coord, 0_i ).r() );

					IF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.discard();
					}
					FI;

					auto accum = writer.declLocale( "accum"
						, c3d_mapAccumulation.fetch( coord, 0_i ) );

					// Suppress overflow
					IF( writer, sdw::isinf( maxComponent( sdw::abs( accum.rgb() ) ) ) )
					{
						accum.rgb() = vec3( accum.a() );
					}
					FI;

					auto averageColor = writer.declLocale( "averageColor"
						, accum.rgb() / max( accum.a(), 0.00001_f ) );

					pxl_fragColor = vec4( averageColor.rgb(), 1.0_f - revealage );

					IF( writer, c3d_sceneData.fogType != UInt( uint32_t( FogType::eDisabled ) ) )
					{
						auto texCoord = writer.declLocale( "texCoord"
							, in.fragCoord.xy() );
						auto position = writer.declLocale( "position"
							, c3d_gpInfoData.projToView( utils
								, texCoord
								, c3d_mapDepth.sample( texCoord ).r() ) );
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, pxl_fragColor
							, length( position )
							, position.z()
							, c3d_sceneData );
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		crg::ImageViewId createDepthOnlyView( crg::FrameGraph & graph
			, crg::ImageViewId depthStencilView )
		{
			auto subresourceRange = depthStencilView.data->info.subresourceRange;
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			return graph.createView( crg::ImageViewData{ depthStencilView.data->image.data->name + "DptOnly"
				, depthStencilView.data->image
				, depthStencilView.data->info.flags
				, depthStencilView.data->info.viewType
				, depthStencilView.data->info.format
				, subresourceRange } );
		}
	}

	//*********************************************************************************************

	WeightedBlendRendering::WeightedBlendRendering( crg::FrameGraph & graph
		, RenderDevice const & device
		, crg::FramePass const & transparentPassDesc
		, TransparentPassResult const & transparentPassResult
		, crg::ImageViewId const & targetColourView
		, castor::Size const & size
		, SceneUbo & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo )
		: m_device{ device }
		, m_graph{ graph }
		, m_transparentPassResult{ transparentPassResult }
		, m_depthOnlyView{ m_transparentPassResult[WbTexture::eDepth].sampledViewId }
		, m_size{ size }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "TransparentCombine", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "TransparentCombine", getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_finalCombinePassDesc{ doCreateFinalCombine( graph
			, transparentPassDesc
			, targetColourView
			, sceneUbo
			, hdrConfigUbo
			, gpInfoUbo ) }
	{
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Transparent Accumulation"
			, m_transparentPassResult[WbTexture::eAccumulation]
			, m_graph.getFinalLayout( m_transparentPassResult[WbTexture::eAccumulation].sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Transparent Revealage"
			, m_transparentPassResult[WbTexture::eRevealage]
			, m_graph.getFinalLayout( m_transparentPassResult[WbTexture::eRevealage].sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	crg::FramePass & WeightedBlendRendering::doCreateFinalCombine( crg::FrameGraph & graph
		, crg::FramePass const & transparentPassDesc
		, crg::ImageViewId const & targetColourView
		, SceneUbo & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo )
	{
		auto & result = graph.createPass( "TransparentCombine"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( m_size ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( pass, context, graph );
				m_device.renderSystem.getEngine()->registerTimer( m_graph.getName()
					, result->getTimer() );
				return result;
			} );
		result.addDependency( transparentPassDesc );
		sceneUbo.createPassBinding( result
			, uint32_t( SceneUboIndex ) );
		gpInfoUbo.createPassBinding( result
			, uint32_t( GpuInfoUboIndex ) );
		hdrConfigUbo.createPassBinding( result
			, uint32_t( HdrUboIndex ) );
		result.addSampledView( m_depthOnlyView
			, uint32_t( DepthTexIndex )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		result.addSampledView( m_transparentPassResult[WbTexture::eAccumulation].sampledViewId
			, uint32_t( AccumTexIndex ) );
		result.addSampledView( m_transparentPassResult[WbTexture::eRevealage].sampledViewId
			, uint32_t( RevealTexIndex ) );

		result.addInOutColourView( targetColourView
			, { VK_TRUE
				, VK_BLEND_FACTOR_SRC_ALPHA
				, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_SRC_ALPHA
				, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
				, VK_BLEND_OP_ADD
				, defaultColorWriteMask } );

		return result;
	}
}
