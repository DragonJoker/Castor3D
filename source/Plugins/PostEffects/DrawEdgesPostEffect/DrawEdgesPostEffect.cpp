#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>

#include <Shaders/GlslToonProfile.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace draw_edges
{
	namespace px
	{
		namespace c3d = castor3d::shader;

		enum Idx : uint32_t
		{
			eMaterials,
			eModels,
			eDepthObj,
			eSource,
			eScattering,
			eEdgeDN,
			eEdgeO,
			eDrawEdges,
			eSpecifics,
		};

		static castor3d::ShaderPtr getProgram( castor3d::Engine & engine
			, VkExtent3D const & extent )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			castor3d::shader::Utils utils{ writer };
			castor3d::shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, castor3d::TextureCombine{}
				, castor3d::ComponentModeFlag::eNone
				, utils };

			auto specifics = uint32_t( eSpecifics );
			castor3d::shader::Materials materials{ engine, writer, passShaders, eMaterials, 0u, specifics };
			C3D_ModelsData( writer, eModels, 0u );
			auto c3d_depthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObj", eDepthObj, 0u );
			auto c3d_source = writer.declCombinedImg< FImg2DRgba32 >( "c3d_source", eSource, 0u );
			auto c3d_scattering = writer.declCombinedImg< FImg2DRgba32 >( "c3d_scattering", eScattering, 0u );
			auto c3d_edgeDN = writer.declCombinedImg< FImg2DR32 >( "c3d_edgeDN", eEdgeDN, 0u );
			auto c3d_edgeO = writer.declCombinedImg< FImg2DR32 >( "c3d_edgeO", eEdgeO, 0u );
			C3D_DrawEdges( writer, eDrawEdges, 0u );

			auto getEdge = writer.implementFunction< sdw::Float >( "getEdge"
				, [&]( sdw::CombinedImage2DR32 const & tex
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
				, sdw::InCombinedImage2DR32{ writer, "tex" }
				, sdw::InIVec2{ writer, "texCoord" }
				, sdw::InInt{ writer, "width" } );

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position().xy(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto colour = writer.declLocale( "colour"
						, c3d_source.sample( in.uv() ) );

					auto size = writer.declLocale( "size"
						, ivec2( sdw::Int{ int( extent.width ) }, sdw::Int{ int( extent.height ) } ) );
					auto texelCoord = writer.declLocale( "texelCoord"
						, ivec2( vec2( size ) * in.uv() ) );

					auto depthObj = writer.declLocale( "depthObj"
						, c3d_depthObj.fetch( texelCoord, 0_i ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					IF( writer, nodeId != 0_u )
					{
						auto modelData = writer.declLocale( "modelData"
							, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
						auto & toonProfiles = materials.getSpecificsBuffer< toon::shader::ToonProfile >();
						auto toonProfile = writer.declLocale( "toonProfile"
							, toonProfiles.getData( modelData.getMaterialId() - 1u ) );

						IF( writer, toonProfile.edgeColour().a() != 0.0_f )
						{
							auto edgeDN = writer.declLocale( "edgeDN"
								, writer.ternary( c3d_drawEdgesData.normalDepthWidth > 0_i
									, getEdge( c3d_edgeDN, texelCoord, c3d_drawEdgesData.normalDepthWidth )
									, 0.0_f ) );
							auto edgeO = writer.declLocale( "edgeO"
								, writer.ternary( c3d_drawEdgesData.objectWidth > 0_i
									, getEdge( c3d_edgeO, texelCoord, c3d_drawEdgesData.objectWidth )
									, 0.0_f ) );

							auto edgeColour = writer.declLocale( "edgeColour"
								, toonProfile.edgeColour().rgb() + c3d_scattering.fetch( texelCoord, 0_i ).rgb() );
							auto edge = writer.declLocale( "edge"
								, mix( colour.rgb(), edgeColour, vec3( edgeDN ) ) );
							// outline contour over inline
							edge = mix( edge, edgeColour, vec3( edgeO ) );

							colour.rgb() = mix( colour.rgb(), edge, vec3( toonProfile.edgeColour().a() ) );
						}
						FI;
					}
					FI;

					out.colour() = colour;
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	const castor::MbString PostEffect::Name = "Draw Edges PostEffect";
	const castor::String PostEffect::Type = cuT( "draw_edges" );
	const castor::String PostEffect::NormalDepthWidth = cuT( "normalDepthWidth" );
	const castor::String PostEffect::ObjectWidth = cuT( "objectWidth" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, cuT( "DrawEdges" )
			, castor::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, parameters
			, 1u }
		, m_shader{ cuT( "DECombine" ), px::getProgram( *renderTarget.getEngine()
			, castor3d::getSafeBandedExtent3D( m_renderTarget.getSize() ) ) }
		, m_stages{ makeProgramStates( renderSystem.getRenderDevice(), m_shader ) }
		, m_ubo{ renderSystem.getRenderDevice() }
	{
		setParameters( parameters );
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectUPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return castor::makeUniqueDerived< castor3d::PostEffect, PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		if ( m_depthNormal )
		{
			m_depthNormal->accept( visitor );
		}

		if ( m_objectID )
		{
			m_objectID->accept( visitor );
		}

		visitor.visit( m_shader );
		visitor.visit( cuT( "NormalDepthWidth" )
			, m_config.normalDepthWidth );
		visitor.visit( cuT( "ObjectWidth" )
			, m_config.objectWidth );
	}

	void PostEffect::setParameters( castor3d::Parameters parameters )
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

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::Texture const & source
		, castor3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		auto & engine = *device.renderSystem.getEngine();
		auto & technique = m_renderTarget.getTechnique();
		auto & passBuffer = engine.getMaterialCache().getPassBuffer();
		auto & depthObj = technique.getDepthObj().sampledViewId;
		auto & nmlOcc = technique.getNormal().sampledViewId;
		auto & depthRange = technique.getDepthRange();
		auto previous = &previousPass;

		m_depthNormal = castor::make_unique< DepthNormalEdgeDetection >( m_graph
			, crg::FramePassArray{ previous, &m_renderTarget.getTechnique().getDepthRangePass() }
			, m_renderTarget
			, device
			, passBuffer
			, depthObj
			, nmlOcc
			, depthRange
			, &isEnabled() );
		m_objectID = castor::make_unique< ObjectIDEdgeDetection >( m_graph
			, *previous
			, m_renderTarget
			, device
			, passBuffer
			, depthObj
			, &isEnabled() );
		previous = &m_objectID->getPass();

		auto extent = castor3d::makeExtent2D( target.getExtent() );
		auto & pass = m_graph.createPass( "Combine"
			, [this, &device, extent]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( extent )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &isEnabled() )
					.passIndex( &m_passIndex )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ 2u } );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		auto & modelBuffer = m_renderTarget.getScene()->getModelBuffer().getBuffer();
		pass.addDependency( m_depthNormal->getPass() );
		pass.addDependency( m_objectID->getPass() );
		passBuffer.createPassBinding( pass, px::eMaterials );
		pass.addInputStorageBuffer( { modelBuffer, "Models" }
			, uint32_t( px::eModels )
			, 0u
			, uint32_t( modelBuffer.getSize() ) );
		pass.addSampledView( depthObj, px::eDepthObj );
		pass.addSampledView( crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }, px::eSource );
		pass.addSampledView( technique.getScattering().sampledViewId, px::eScattering );
		pass.addSampledView( m_depthNormal->getResult(), px::eEdgeDN );
		pass.addSampledView( m_objectID->getResult(), px::eEdgeO );
		m_ubo.createPassBinding( pass, px::eDrawEdges );
		auto index = uint32_t( px::eSpecifics );
		device.renderSystem.getEngine()->createSpecificsBuffersPassBindings( pass, index );
		pass.addOutputColourView( crg::ImageViewIdArray{ target.targetViewId, source.targetViewId } );

		m_pass = &pass;
		return true;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_objectID.reset();
		m_depthNormal.reset();
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
