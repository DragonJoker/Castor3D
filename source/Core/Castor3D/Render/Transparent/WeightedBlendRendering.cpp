#include "Castor3D/Render/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

CU_ImplementSmartPtr( castor3d, WeightedBlendRendering )

namespace castor3d
{
	//*********************************************************************************************

	namespace wboit
	{
		enum TransparentResolveIdx
		{
			CameraUboIndex,
			SceneUboIndex,
			HdrUboIndex,
			DepthTexIndex,
			AccumTexIndex,
			RevealTexIndex,
		};

		static ShaderPtr getProgram( RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			shader::Utils utils{ writer };
			shader::Fog fog{ writer };

			C3D_Camera( writer, CameraUboIndex, 0u );
			C3D_Scene( writer, SceneUboIndex, 0u );
			C3D_HdrConfig( writer, HdrUboIndex, 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", uint32_t( DepthTexIndex ), 0u );
			auto c3d_mapAccumulation = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( WbTexture::eAccumulation ), uint32_t( AccumTexIndex ), 0u );
			auto c3d_mapRevealage = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( WbTexture::eRevealage ), uint32_t( RevealTexIndex ), 0u );

			writer.implementEntryPointT< shader::PosUv2FT, sdw::VoidT >( [&]( sdw::VertexInT< shader::PosUv2FT > in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			auto maxComponent = writer.implementFunction< sdw::Float >( "maxComponent"
				, [&]( sdw::Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
				, sdw::InVec3{ writer, "v" } );

			writer.implementEntryPointT< sdw::VoidT, shader::Colour4FT >( [&]( sdw::FragmentIn in
				, sdw::FragmentOutT< shader::Colour4FT > out )
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

					out.colour() = vec4( averageColor.rgb(), 1.0_f - revealage );

					IF( writer, c3d_sceneData.fogType() != sdw::UInt( uint32_t( FogType::eDisabled ) ) )
					{
						auto texCoord = writer.declLocale( "texCoord"
							, in.fragCoord.xy() );
						auto position = writer.declLocale( "position"
							, c3d_cameraData.curProjToWorld( utils
								, texCoord
								, c3d_mapDepth.sample( texCoord ).r() ) );
						out.colour() = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, out.colour()
							, position
							, c3d_cameraData.position()
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
		, bool & enabled
		, crg::FramePass const & transparentPassDesc
		, Texture const & depthObj
		, TransparentPassResult const & transparentPassResult
		, crg::ImageViewIdArray const & targetColourView
		, castor::Size const & size
		, CameraUbo const & cameraUbo
		, SceneUbo const & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo )
		: m_device{ device }
		, m_graph{ graph }
		, m_enabled{ enabled }
		, m_transparentPassResult{ transparentPassResult }
		, m_size{ size }
		, m_shader{ "TransparentCombine", wboit::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_finalCombinePassDesc{ doCreateFinalCombine( graph
			, transparentPassDesc
			, depthObj.sampledViewId
			, targetColourView
			, cameraUbo
			, sceneUbo
			, hdrConfigUbo
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
		visitor.visit( m_shader );
	}

	crg::FramePass & WeightedBlendRendering::doCreateFinalCombine( crg::FramePassGroup & graph
		, crg::FramePass const & transparentPassDesc
		, crg::ImageViewId const & depthObj
		, crg::ImageViewIdArray const & targetColourView
		, CameraUbo const & cameraUbo
		, SceneUbo const & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
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
					.build( framePass, context, graph, crg::ru::Config{ 1u } );
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		result.addDependency( transparentPassDesc );
		cameraUbo.createPassBinding( result
			, uint32_t( wboit::CameraUboIndex ) );
		sceneUbo.createPassBinding( result
			, uint32_t( wboit::SceneUboIndex ) );
		hdrConfigUbo.createPassBinding( result
			, uint32_t( wboit::HdrUboIndex ) );
		result.addSampledView( depthObj
			, uint32_t( wboit::DepthTexIndex ) );
		result.addSampledView( m_transparentPassResult[WbTexture::eAccumulation].sampledViewId
			, uint32_t( wboit::AccumTexIndex ) );
		result.addSampledView( m_transparentPassResult[WbTexture::eRevealage].sampledViewId
			, uint32_t( wboit::RevealTexIndex ) );

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
