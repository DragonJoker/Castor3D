#include "FxaaPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <FrameBuffer/BackBuffers.hpp>
#include <FrameBuffer/FrameBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace fxaa
{
	namespace
	{
		static String const FxaaUbo = cuT( "FxaaUbo" );
		static String const SubpixShift = cuT( "c3d_fSubpixShift" );
		static String const RenderTargetWidth = cuT( "c3d_fRenderTargetWidth" );
		static String const RenderTargetHeight = cuT( "c3d_fRenderTargetHeight" );
		static String const SpanMax = cuT( "c3d_fSpanMax" );
		static String const ReduceMul = cuT( "c3d_fReduceMul" );
		static String const PosPos = cuT( "vtx_posPos" );

		glsl::Shader getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_FXAA( writer );
			auto position = writer.declAttribute< Vec2 >( castor3d::ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = writer.declOutput< Vec4 >( PosPos );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto invTargetSize = writer.declLocale( cuT( "invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );
					vtx_texture = position;
					gl_Position = c3d_projection * vec4( position.xy(), 0.0, 1.0 );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - writer.paren( invTargetSize * writer.paren( 0.5 + c3d_subpixShift ) );
				} );
			return writer.finalise();
		}

		glsl::Shader getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( castor3d::ShaderProgram::MapDiffuse, castor3d::MinTextureIndex );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = writer.declInput< Vec4 >( PosPos );

			UBO_FXAA( writer );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

#define FXAA_REDUCE_MIN	( 1.0 / 128.0 )

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto invTargetSize = writer.declLocale( cuT( "invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );

					auto rgbNW = writer.declLocale( cuT( "rgbNW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto rgbNE = writer.declLocale( cuT( "rgbNE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto rgbSW = writer.declLocale( cuT( "rgbSW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto rgbSE = writer.declLocale( cuT( "rgbSE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto rgbM = writer.declLocale( cuT( "rgbM" )
						, texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).rgb() );

					auto luma = writer.declLocale( cuT( "luma" )
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto lumaNW = writer.declLocale( cuT( "lumaNW" )
						, dot( rgbNW, luma ) );
					auto lumaNE = writer.declLocale( cuT( "lumaNE" )
						, dot( rgbNE, luma ) );
					auto lumaSW = writer.declLocale( cuT( "lumaSW" )
						, dot( rgbSW, luma ) );
					auto lumaSE = writer.declLocale( cuT( "lumaSE" )
						, dot( rgbSE, luma ) );
					auto lumaM = writer.declLocale( cuT( "lumaM" )
						, dot( rgbM, luma ) );

					auto lumaMin = writer.declLocale( cuT( "lumaMin" )
						, min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
					auto lumaMax = writer.declLocale( cuT( "lumaMax" )
						, max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );

					auto dir = writer.declLocale( cuT( "dir" )
						, vec2( -writer.paren( writer.paren( lumaNW + lumaNE ) - writer.paren( lumaSW + lumaSE ) )
							, ( writer.paren( lumaNW + lumaSW ) - writer.paren( lumaNE + lumaSE ) ) ) );

					auto dirReduce = writer.declLocale( cuT( "dirReduce" )
						, max( writer.paren( lumaNW + lumaNE + lumaSW + lumaSE ) * writer.paren( 0.25_f * c3d_reduceMul ), FXAA_REDUCE_MIN ) );
					auto rcpDirMin = writer.declLocale( cuT( "rcpDirMin" )
						, 1.0 / ( min( glsl::abs( dir.x() ), glsl::abs( dir.y() ) ) + dirReduce ) );
					dir = min( vec2( c3d_spanMax, c3d_spanMax )
						, max( vec2( -c3d_spanMax, -c3d_spanMax )
							, dir * rcpDirMin ) ) * invTargetSize;

					auto rgbA = writer.declLocale( cuT( "rgbA" )
						, writer.paren( texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 1.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
								+ texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 2.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
							* writer.paren( 1.0_f / 2.0_f ) );
					auto rgbB = writer.declLocale( cuT( "rgbB" )
						, writer.paren( rgbA * 1.0_f / 2.0_f )
							+ writer.paren( texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 0.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
									+ texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 3.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
								* writer.paren( 1.0_f / 4.0_f ) );
					auto lumaB = writer.declLocale( cuT( "lumaB" )
						, dot( rgbB, luma ) );

					pxl_fragColor = vec4( writer.ternary( Type{ cuT( "lumaB < lumaMin || lumaB > lumaMax" ) }, rgbA, rgbB ), 1.0_f );
				} );

			return writer.finalise();
		}
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "fxaa" );
	String PostEffect::Name = cuT( "FXAA PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & p_renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & p_parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, p_renderTarget
			, renderSystem
			, p_parameters }
		, m_surface{ *renderSystem.getEngine() }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_fxaaUbo{ *renderSystem.getEngine() }
	{
		String param;

		if ( p_parameters.get( cuT( "SubpixShift" ), param ) )
		{
			m_subpixShift = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "MaxSpan" ), param ) )
		{
			m_spanMax = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "ReduceMul" ), param ) )
		{
			m_reduceMul = string::toFloat( param );
		}

		String name = cuT( "FXAA" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler->setInterpolationMode( castor3d::InterpolationFilter::eMin, castor3d::InterpolationMode::eNearest );
			m_sampler->setInterpolationMode( castor3d::InterpolationFilter::eMag, castor3d::InterpolationMode::eNearest );
			m_sampler->setWrappingMode( castor3d::TextureUVW::eU, castor3d::WrapMode::eClampToBorder );
			m_sampler->setWrappingMode( castor3d::TextureUVW::eV, castor3d::WrapMode::eClampToBorder );
			m_sampler->setWrappingMode( castor3d::TextureUVW::eW, castor3d::WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & p_renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & p_param )
	{
		return std::make_shared< PostEffect >( p_renderTarget, renderSystem, p_param );
	}

	bool PostEffect::initialise()
	{
		bool result = false;
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		Size size = m_renderTarget.getSize();

		auto vertex = getVertexProgram( getRenderSystem() );
		auto fragment = getFragmentProgram( getRenderSystem() );

		auto program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->createUniform< castor3d::UniformType::eSampler >( castor3d::ShaderProgram::MapDiffuse
			, castor3d::ShaderType::ePixel )->setValue( castor3d::MinTextureIndex );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, fragment );
		program->initialise();

		castor3d::DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( castor3d::WritingMask::eZero );
		castor3d::RasteriserState rsstate;
		rsstate.setCulledFaces( castor3d::Culling::eBack );
		m_pipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, castor3d::BlendState{}
			, castor3d::MultisampleState{}
			, *program
			, castor3d::PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_fxaaUbo.getUbo() );

		return m_surface.initialise( m_renderTarget, size, 0, m_sampler );
	}

	void PostEffect::cleanup()
	{
		m_matrixUbo.getUbo().cleanup();
		m_fxaaUbo.getUbo().cleanup();
		m_surface.cleanup();
	}

	bool PostEffect::apply( castor3d::FrameBuffer & p_framebuffer )
	{
		auto attach = p_framebuffer.getAttachment( castor3d::AttachmentPoint::eColour, 0 );

		if ( attach && attach->getAttachmentType() == castor3d::AttachmentType::eTexture )
		{
			m_fxaaUbo.update( attach->getBuffer()->dimensions()
				, m_subpixShift
				, m_spanMax
				, m_reduceMul );
			m_pipeline->apply();
			m_surface.m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
			auto texture = std::static_pointer_cast< castor3d::TextureAttachment >( attach )->getTexture();
			m_surface.m_fbo->clear( castor3d::BufferComponent::eColour );
			getRenderSystem()->getCurrentContext()->renderTexture( m_surface.m_size
				, *texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->unbind();

			p_framebuffer.bind( castor3d::FrameBufferTarget::eDraw );
			getRenderSystem()->getCurrentContext()->renderTexture( texture->getDimensions(), *m_surface.m_colourTexture.getTexture() );
			p_framebuffer.unbind();
		}

		return true;
	}

	bool PostEffect::doWriteInto( TextFile & p_file )
	{
		return true;
	}
}
