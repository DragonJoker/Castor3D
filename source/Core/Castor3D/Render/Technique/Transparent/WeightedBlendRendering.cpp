#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
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

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram( RenderSystem const & renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_SCENE( writer, uint32_t( SceneUboIndex ), 0u );
			UBO_GPINFO( writer, uint32_t( GpuInfoUboIndex ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( HdrUboIndex ), 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( WbTexture::eDepth ), uint32_t( DepthTexIndex ), 0u );
			auto c3d_mapAccumulation = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( WbTexture::eAccumulation ), uint32_t( AccumTexIndex ), 0u );
			auto c3d_mapRevealage = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( WbTexture::eRevealage ), uint32_t( RevealTexIndex ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			shader::Utils utils{ writer, *renderSystem.getEngine() };
			shader::Fog fog{ writer };

			auto maxComponent = writer.implementFunction< Float >( "maxComponent"
				, [&]( Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
				, InVec3{ writer, "v" } );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto coord = writer.declLocale( "coord"
						, ivec2( in.fragCoord.xy() ) );
					auto revealage = writer.declLocale( "revealage"
						, c3d_mapRevealage.fetch( coord, 0_i ).r() );

					IF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.demote();
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
							, c3d_gpInfoData.projToWorld( utils
								, texCoord
								, c3d_mapDepth.sample( texCoord ).r() ) );
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, pxl_fragColor
							, position
							, c3d_sceneData );
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	WeightedBlendRendering::WeightedBlendRendering( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
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
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "TransparentCombine", getPixelProgram( device.renderSystem ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_finalCombinePassDesc{ doCreateFinalCombine( graph
			, transparentPassDesc
			, targetColourView
			, sceneUbo
			, hdrConfigUbo
			, gpInfoUbo
			, progress ) }
	{
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Transparent Accumulation"
			, m_transparentPassResult[WbTexture::eAccumulation]
			, m_graph.getFinalLayoutState( m_transparentPassResult[WbTexture::eAccumulation].sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Transparent Revealage"
			, m_transparentPassResult[WbTexture::eRevealage]
			, m_graph.getFinalLayoutState( m_transparentPassResult[WbTexture::eRevealage].sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	crg::FramePass & WeightedBlendRendering::doCreateFinalCombine( crg::FramePassGroup & graph
		, crg::FramePass const & transparentPassDesc
		, crg::ImageViewId const & targetColourView
		, SceneUbo & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating transparent resolve pass" );
		auto & result = graph.createPass( "Combine"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising transparent resolve pass" );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( m_size ) )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &m_enabled )
					.build( framePass, context, graph );
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
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
			, uint32_t( DepthTexIndex ) );
		result.addSampledView( m_transparentPassResult[WbTexture::eAccumulation].sampledViewId
			, uint32_t( AccumTexIndex )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addSampledView( m_transparentPassResult[WbTexture::eRevealage].sampledViewId
			, uint32_t( RevealTexIndex )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

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
