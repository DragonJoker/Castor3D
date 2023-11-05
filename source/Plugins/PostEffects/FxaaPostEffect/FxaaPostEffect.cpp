#include "FxaaPostEffect/FxaaPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace fxaa
{
	namespace postfx
	{
		namespace c3d = castor3d::shader;

		static castor::String const PosPos = cuT( "vtx_posPos" );

		enum Idx : uint32_t
		{
			FxaaCfgUboIdx,
			ColorTexIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::Engine & engine )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			sdw::Float fxaaReduceMin{ 1.0_f / 128.0_f };

			C3D_Fxaa( writer, FxaaCfgUboIdx, 0u );
			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", ColorTexIdx, 0u );

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::PosUv4FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::PosUv4FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position().xy(), 0.0_f, 1.0_f );
					out.position().xy() = in.position().xy();
					out.position().zw() = in.position().xy() - ( c3d_fxaaData.pixelSize * ( 0.5_f + c3d_fxaaData.subpixShift ) );
				} );

			writer.implementEntryPointT< c3d::PosUv4FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::PosUv4FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto rgbNW = writer.declLocale( "rgbNW"
						, c3d_mapColor.lod( in.uv(), 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto rgbNE = writer.declLocale( "rgbNE"
						, c3d_mapColor.lod( in.uv(), 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto rgbSW = writer.declLocale( "rgbSW"
						, c3d_mapColor.lod( in.uv(), 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto rgbSE = writer.declLocale( "rgbSE"
						, c3d_mapColor.lod( in.uv(), 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto rgbM = writer.declLocale( "rgbM"
						, c3d_mapColor.sample( in.uv(), 0.0_f ).rgb() );

					auto luma = writer.declLocale( "luma"
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto lumaNW = writer.declLocale( "lumaNW"
						, dot( rgbNW, luma ) );
					auto lumaNE = writer.declLocale( "lumaNE"
						, dot( rgbNE, luma ) );
					auto lumaSW = writer.declLocale( "lumaSW"
						, dot( rgbSW, luma ) );
					auto lumaSE = writer.declLocale( "lumaSE"
						, dot( rgbSE, luma ) );
					auto lumaM = writer.declLocale( "lumaM"
						, dot( rgbM, luma ) );

					auto lumaMin = writer.declLocale( "lumaMin"
						, min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
					auto lumaMax = writer.declLocale( "lumaMax"
						, max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );

					auto dir = writer.declLocale( "dir"
						, vec2( -( ( lumaNW + lumaNE ) - ( lumaSW + lumaSE ) )
							, ( ( lumaNW + lumaSW ) - ( lumaNE + lumaSE ) ) ) );

					auto dirReduce = writer.declLocale( "dirReduce"
						, max( ( lumaNW + lumaNE + lumaSW + lumaSE ) * ( 0.25_f * c3d_fxaaData.reduceMul ), fxaaReduceMin ) );
					auto rcpDirMin = writer.declLocale( "rcpDirMin"
						, 1.0_f / ( min( abs( dir.x() ), abs( dir.y() ) ) + dirReduce ) );
					dir = min( vec2( c3d_fxaaData.spanMax, c3d_fxaaData.spanMax )
						, max( vec2( -c3d_fxaaData.spanMax, -c3d_fxaaData.spanMax )
							, dir * rcpDirMin ) ) * c3d_fxaaData.pixelSize;

					auto texcoord0 = writer.declLocale( "texture0"
						, in.uv() + dir * ( 1.0_f / 3.0_f - 0.5_f ) );
					auto texcoord1 = writer.declLocale( "texture1"
						, in.uv() + dir * ( 2.0_f / 3.0_f - 0.5_f ) );

					auto rgbA = writer.declLocale( "rgbA"
						, ( c3d_mapColor.sample( texcoord0, 0.0_f ).rgb()
								+ c3d_mapColor.sample( texcoord1, 0.0_f ).rgb() )
							* ( 1.0_f / 2.0_f ) );

					texcoord0 = in.uv() + dir * ( 0.0_f / 3.0_f - 0.5_f );
					texcoord1 = in.uv() + dir * ( 3.0_f / 3.0_f - 0.5_f );

					auto rgbB = writer.declLocale( "rgbB"
						, ( rgbA * 1.0_f / 2.0_f )
							+ ( c3d_mapColor.sample( texcoord0, 0.0_f ).rgb()
									+ c3d_mapColor.sample( texcoord1, 0.0_f ).rgb() )
								* ( 1.0_f / 4.0_f ) );
					auto lumaB = writer.declLocale( "lumaB"
						, dot( rgbB, luma ) );

					out.colour() = vec4( writer.ternary( lumaB < lumaMin || lumaB > lumaMax, rgbA, rgbB )
						, 1.0_f );
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "fxaa" );
	castor::String PostEffect::Name = cuT( "FXAA PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, "FXAA"
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, 1u }
		, m_shader{ "Fxaa", postfx::getProgram( *renderTarget.getEngine() ) }
		, m_stages{ makeProgramStates( renderSystem.getRenderDevice(), m_shader ) }
		, m_fxaaUbo{ renderSystem.getRenderDevice(), m_renderTarget.getSize() }
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
		visitor.visit( m_shader );
		visitor.visit( cuT( "Sub-pixel shift" )
			, m_subpixShift );
		visitor.visit( cuT( "Span max." )
			, m_spanMax );
		visitor.visit( cuT( "Reduce mul." )
			, m_reduceMul );
	}

	void PostEffect::setParameters( castor3d::Parameters parameters )
	{
		castor::String param;

		if ( parameters.get( cuT( "SubpixShift" ), param ) )
		{
			m_subpixShift = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "MaxSpan" ), param ) )
		{
			m_spanMax = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "ReduceMul" ), param ) )
		{
			m_reduceMul = castor::string::toFloat( param );
		}
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::Texture const & source
		, castor3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		auto extent = castor3d::makeExtent2D( target.getExtent() );
		m_pass = &m_graph.createPass( "FXAA"
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
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		m_pass->addDependency( previousPass );
		m_fxaaUbo.createPassBinding( *m_pass
			, postfx::FxaaCfgUboIdx );
		m_pass->addSampledView( crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, postfx::ColorTexIdx );
		m_pass->addOutputColourView( crg::ImageViewIdArray{ target.targetViewId, source.targetViewId } );
		return true;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
	{
		if ( m_subpixShift.isDirty()
			|| m_spanMax.isDirty()
			|| m_reduceMul.isDirty() )
		{
			m_fxaaUbo.cpuUpdate( m_subpixShift
				, m_spanMax
				, m_reduceMul );
			m_subpixShift.reset();
			m_spanMax.reset();
			m_reduceMul.reset();
		}
	}

	bool PostEffect::doWriteInto( castor::StringStream & file
		, castor::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"\n" ) );
		return true;
	}
}
