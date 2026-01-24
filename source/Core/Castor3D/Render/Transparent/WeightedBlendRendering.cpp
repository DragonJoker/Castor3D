#include "Castor3D/Render/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

CU_ImplementSmartPtr( c3d, WeightedBlendRendering )

namespace c3d
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
			C3D_Render( writer, HdrUboIndex, 0u );
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", uint32_t( DepthTexIndex ), 0u );
			auto c3d_mapAccumulation = writer.declCombinedImg< FImg2DRgba16 >( toUtf8( getTextureName( WbTexture::eAccumulation ) ), uint32_t( AccumTexIndex ), 0u );
			auto c3d_mapRevealage = writer.declCombinedImg< FImg2DR16 >( toUtf8( getTextureName( WbTexture::eRevealage ) ), uint32_t( RevealTexIndex ), 0u );

			writer.implementEntryPointT< shader::PosUv2FT, sdw::VoidT >( [&]( sdw::VertexInT< shader::PosUv2FT > const & in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			auto maxComponent = writer.implementFunction< sdw::Float >( "maxComponent"
				, [&writer]( sdw::Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
				, sdw::InVec3{ writer, "v" } );

			writer.implementEntryPointT< sdw::VoidT, shader::Colour4FT >( [&]( sdw::FragmentIn const & in
				, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					auto coord = writer.declLocale( "coord"
						, ivec2( in.fragCoord.xy() ) );
					auto revealage = writer.declLocale( "revealage"
						, c3d_mapRevealage.fetch( coord, 0_i ) );

					sdwIF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.demote();
					}
					sdwFI

					auto accum = writer.declLocale( "accum"
						, c3d_mapAccumulation.fetch( coord, 0_i ) );

					// Suppress overflow
					sdwIF( writer, sdw::isinf( maxComponent( sdw::abs( accum.rgb() ) ) ) )
					{
						accum.rgb() = vec3( accum.a() );
					}
					sdwFI

					auto averageColor = writer.declLocale( "averageColor"
						, accum.rgb() / max( accum.a(), 0.00001_f ) );

					out.colour() = vec4( averageColor.rgb(), 1.0_f - revealage );

					sdwIF( writer, c3d_sceneData.fogType() != sdw::UInt( uint32_t( FogType::eDisabled ) ) )
					{
						auto texCoord = writer.declLocale( "texCoord"
							, in.fragCoord.xy() );
						auto position = writer.declLocale( "position"
							, c3d_cameraData.curProjToWorld( utils
								, texCoord
								, c3d_mapDepth.sample( texCoord ).r() ) );
						out.colour() = fog.apply( c3d_sceneData.getBackgroundColour( c3d_renderData )
							, out.colour()
							, position
							, c3d_cameraData.position()
							, c3d_sceneData );
					}
					sdwFI
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	WeightedBlendRendering::WeightedBlendRendering( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, bool & enabled
		, Texture const & depthObj
		, TransparentPassResult const & transparentPassResult
		, Texture & targetColour
		, Size const & size
		, CameraUbo const & cameraUbo
		, SceneUbo const & sceneUbo
		, RenderUbo const & renderUbo )
		: m_device{ device }
		, m_graph{ graph }
		, m_enabled{ enabled }
		, m_transparentPassResult{ transparentPassResult }
		, m_size{ size }
		, m_shader{ cuT( "TransparentCombine" ), wboit::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		stepProgressBarLocal( progress, cuT( "Creating transparent resolve pass" ) );
		auto & pass = graph.createPass( "Combine"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising transparent resolve pass" ) );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( m_size ) )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &m_enabled )
					.build( framePass, context, runGraph, crg::ru::Config{ 1u } );
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		cameraUbo.createPassBinding( pass, uint32_t( wboit::CameraUboIndex ) );
		sceneUbo.createPassBinding( pass, uint32_t( wboit::SceneUboIndex ) );
		renderUbo.createPassBinding( pass, uint32_t( wboit::HdrUboIndex ) );
		pass.addInputSampled( *depthObj.getSampledLastAttach(), uint32_t( wboit::DepthTexIndex ) );
		pass.addInputSampled( *m_transparentPassResult.getSampledLastAttach( WbTexture::eAccumulation ), uint32_t( wboit::AccumTexIndex ) );
		pass.addInputSampled( *m_transparentPassResult.getSampledLastAttach( WbTexture::eRevealage ), uint32_t( wboit::RevealTexIndex ) );

		targetColour.setLastAttach( pass.addInOutColourTarget( *targetColour.getLastAttach()
			, { VK_TRUE
				, BlendFactor::eSrcAlpha, BlendFactor::eOneMinusSrcAlpha, BlendOp::eAdd
				, BlendFactor::eSrcAlpha, BlendFactor::eOneMinusSrcAlpha, BlendOp::eAdd
				, defaultColorWriteMask } ) );
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "Transparent Accumulation" )
			, m_transparentPassResult.getTexture( WbTexture::eAccumulation )
			, m_graph.getFinalLayoutState( m_transparentPassResult.getSampledViewId( WbTexture::eAccumulation ) ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Transparent Revealage" )
			, m_transparentPassResult.getTexture( WbTexture::eRevealage )
			, m_graph.getFinalLayoutState( m_transparentPassResult.getSampledViewId( WbTexture::eRevealage ) ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( m_shader );
	}

	uint32_t WeightedBlendRendering::countInitialisationSteps()noexcept
	{
		uint32_t result = 0u;
		result += 1;// transparent resolve pass;
		return result;
	}
}
