#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace draw_edges
{
	namespace
	{
		enum Idx : uint32_t
		{
			eMaterials,
			eData0,
			eData1,
			eSource,
			eEdgeDN,
			eEdgeO,
			eDrawEdges,
		};

		std::unique_ptr< ast::Shader > getVertexProgram()
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
					out.vtx.position = vec4( position.xy(), 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getFragmentProgram( VkExtent3D const & extent )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			castor3d::shader::Materials materials{ writer };
			materials.declare( true, eMaterials, 0u );
			auto c3d_data0 = writer.declSampledImage< FImg2DRgba32 >( "c3d_data0", eData0, 0u );
			auto c3d_source = writer.declSampledImage< FImg2DRgba32 >( "c3d_source", eSource, 0u );
			auto c3d_edgeDN = writer.declSampledImage< FImg2DR32 >( "c3d_edgeDN", eEdgeDN, 0u );
			auto c3d_edgeO = writer.declSampledImage< FImg2DR32 >( "c3d_edgeO", eEdgeO, 0u );
			UBO_DRAW_EDGES( writer, eDrawEdges, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto fragColor = writer.declOutput< Vec4 >( "fragColor", 0 );

			auto getEdge = writer.implementFunction< sdw::Float >( "getEdge"
				, [&]( sdw::SampledImage2DR32 const & tex
					, sdw::IVec2 const & texCoord
					, sdw::Int const & width )
				{
					auto result = writer.declLocale( "result"
						, tex.fetch( texCoord, 0_i ) );

					FOR( writer, sdw::Int, x, 1_i, x < width, ++x )
					{
						result += tex.fetch( texCoord + ivec2( +x, +0_i ), 0_i );
						result += tex.fetch( texCoord + ivec2( -x, +0_i ), 0_i );

						FOR( writer, sdw::Int, y, 1_i, y < width, ++y )
						{
							result += tex.fetch( texCoord + ivec2( +x, +y ), 0_i );
							result += tex.fetch( texCoord + ivec2( -x, -y ), 0_i );
						}
						ROF;
					}
					ROF;

					FOR( writer, sdw::Int, y, 1_i, y < width, ++y )
					{
						result += tex.fetch( texCoord + ivec2( +0_i, +y ), 0_i );
						result += tex.fetch( texCoord + ivec2( +0_i, -y ), 0_i );
					}
					ROF;

					writer.returnStmt( clamp( result, 0.0_f, 1.0_f ) );
				}
				, sdw::InSampledImage2DR32{ writer, "tex" }
				, sdw::InIVec2{ writer, "texCoord" }
				, sdw::InInt{ writer, "width" } );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto colour = writer.declLocale( "colour"
						, c3d_source.sample( vtx_texture ) );

					auto size = writer.declLocale( "size"
						, ivec2( sdw::Int{ int( extent.width ) }, sdw::Int{ int( extent.height ) } ) );
					auto texelCoord = writer.declLocale( "texelCoord"
						, ivec2( vec2( size ) * vtx_texture ) );

					auto data0 = writer.declLocale( "data0"
						, c3d_data0.fetch( texelCoord, 0_i ) );

					IF( writer, data0.w() != 0.0_f  )
					{
						auto material = writer.declLocale( "material"
							, materials.getMaterial( writer.cast< sdw::UInt >( data0.w() ) ) );

						IF( writer, material.edgeColour.a() != 0.0_f )
						{
							auto edgeDN = writer.declLocale( "edgeDN"
								, writer.ternary( c3d_drawEdgesData.normalDepthWidth > 0_i
									, getEdge( c3d_edgeDN, texelCoord, c3d_drawEdgesData.normalDepthWidth )
									, 0.0_f ) );
							auto edgeO = writer.declLocale( "edgeO"
								, writer.ternary( c3d_drawEdgesData.objectWidth > 0_i
									, getEdge( c3d_edgeO, texelCoord, c3d_drawEdgesData.objectWidth )
									, 0.0_f ) );

							auto edge = writer.declLocale( "edge"
								, mix( colour.rgb(), material.edgeColour.rgb(), vec3( edgeDN ) ) );
							// outline contour over inline
							edge = mix( edge, material.edgeColour.rgb(), vec3( edgeO ) );

							colour.rgb() = mix( colour.rgb(), edge, vec3( material.edgeColour.a() ) );
						}
						FI;
					}
					FI;

					fragColor = colour;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	const castor::String PostEffect::Type = cuT( "draw_edges" );
	const castor::String PostEffect::Name = cuT( "Draw Edges PostEffect" );
	const castor::String PostEffect::NormalDepthWidth = cuT( "normalDepthWidth" );
	const castor::String PostEffect::ObjectWidth = cuT( "objectWidth" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, 1u }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "DECombine", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "DECombine", getFragmentProgram( castor3d::getSafeBandedExtent3D( m_renderTarget.getSize() ) ) }
		, m_stages{ makeShaderState( renderSystem.getRenderDevice(), m_vertexShader )
			, makeShaderState( renderSystem.getRenderDevice(), m_pixelShader ) }
		, m_ubo{ renderSystem.getRenderDevice() }
	{
		castor::String param;

		if ( parameters.get( NormalDepthWidth, param ) )
		{
			m_config.normalDepthWidth = castor::string::toInt( param );
		}

		if ( parameters.get( ObjectWidth, param ) )
		{
			m_config.objectWidth = castor::string::toInt( param );
		}
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		if ( m_depthNormal )
		{
			m_depthNormal->accept( visitor );
		}

		if ( m_objectID )
		{
			m_objectID->accept( visitor );
		}

		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( cuT( "NormalDepthWidth" )
			, m_config.normalDepthWidth );
		visitor.visit( cuT( "ObjectWidth" )
			, m_config.objectWidth );
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, crg::FramePass const & previousPass )
	{
		auto extent = castor3d::getSafeBandedExtent3D( m_renderTarget.getSize() );
		auto & engine = *device.renderSystem.getEngine();
		auto & technique = m_renderTarget.getTechnique();
		auto & passBuffer = engine.getMaterialCache().getPassBuffer();
		auto & data0 = technique.getDepthObjImgView();
		auto & data1 = technique.getNormalImgView();
		auto & depthRange = technique.getDepthRange();
		auto previous = &previousPass;

		m_depthNormal = std::make_unique< DepthNormalEdgeDetection >( *previous
			, m_renderTarget
			, device
			, passBuffer
			, data0
			, data1
			, depthRange );
		m_objectID = std::make_unique< ObjectIDEdgeDetection >( *previous
			, m_renderTarget
			, device
			, passBuffer
			, data0 );
		previous = &m_objectID->getPass();

		m_resultImg = m_renderTarget.getGraph().createImage( crg::ImageData{ "DECombineRes"
			, 0u
			, VK_IMAGE_TYPE_2D
			, m_target->data->info.format
			, extent
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) } );
		m_resultView = m_renderTarget.getGraph().createView( crg::ImageViewData{ "DECombineRes"
			, m_resultImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_resultImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		auto & pass = m_renderTarget.getGraph().createPass( "DECombine"
			, [this, &device, extent]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &isEnabled() )
					.build( pass
						, context
						, graph
						, crg::ru::Config{}
							.implicitAction( m_resultView
								, crg::RecordContext::copyImage( *m_target
									, m_resultView
									, { extent.width, extent.height } ) ) );
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/Draw Edges"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( m_depthNormal->getPass() );
		pass.addDependency( m_objectID->getPass() );
		passBuffer.createPassBinding( pass,eMaterials );
		pass.addSampledView( data0, eData0 );
		pass.addSampledView( data1, eData1 );
		pass.addSampledView( *m_target, eSource );
		pass.addSampledView( m_depthNormal->getResult(), eEdgeDN );
		pass.addSampledView( m_objectID->getResult(), eEdgeO );
		m_ubo.createPassBinding( pass, eDrawEdges );
		pass.addOutputColourView( m_resultView );

		m_pass = &pass;
		return &m_resultView;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
	{
		m_ubo.cpuUpdate( m_config.normalDepthWidth
			, m_config.objectWidth );
		auto & technique = m_renderTarget.getTechnique();
		technique.setNeedsDepthRange( isEnabled() );
	}

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		file << ( cuT( "\n" ) + tabs + Type + cuT( "\n" ) );
		file << ( tabs + cuT( "{\n" ) );
		file << ( tabs + cuT( "\t" ) + NormalDepthWidth + cuT( " " ) + castor::string::toString( m_config.normalDepthWidth ) + cuT( "\n" ) );
		file << ( tabs + cuT( "\t" ) + ObjectWidth + cuT( " " ) + castor::string::toString( m_config.objectWidth ) + cuT( "\n" ) );
		file << ( tabs + cuT( "}\n" ) );
		return true;
	}
}
