#include "GaussianBlur.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/Uniform/Uniform.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		glsl::Shader getVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer, 0 );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = position;
					gl_Position = c3d_projection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader getBlurXProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 2u };
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_coefficients = config.declMember< Float >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients );
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			config.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse, MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );
			auto gl_FragDepth = writer.declBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 1.0_f, 0 ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, UInt, i, 1u, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = pxl_fragColor.r();
			} );
			return writer.finalise();
		}

		glsl::Shader getBlurYProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 2u };
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_coefficients = config.declMember< Float >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients );
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			config.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse, MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );
			auto gl_FragDepth = writer.declBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 0.0_f, 1 ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, UInt, i, 1u, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = pxl_fragColor.r();
			} );
			return writer.finalise();
		}

		std::vector< float > getHalfPascal( uint32_t height )
		{
			std::vector< float > result;
			result.resize( height );
			auto x = 1.0f;
			auto max = 1 + height;

			for ( uint32_t i = 0u; i <= max; ++i )
			{
				auto index = max - i;

				if ( index < height )
				{
					result[index] = x;
				}

				x = x * ( ( height + 1 ) * 2 - i ) / ( i + 1 );
			}

			// Normalize kernel coefficients
			float sum = result[0];

			for ( uint32_t i = 1u; i < height; ++i )
			{
				sum += result[i] * 2;
			}

			std::transform( std::begin( result )
				, std::end( result )
				, std::begin( result )
				, [&sum]( float & p_value )
			{
				return p_value /= sum;
			} );

			return result;
		}

		RenderPipelineUPtr doCreatePipeline( Engine & engine
			, ShaderProgram & program )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			return engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, BlendState{}
				, MultisampleState{}
				, program
				, PipelineFlags{} );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, Size const & size
			, PixelFormat format
			, String const & name )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, name );
			auto texture = renderSystem.createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, format
				, size );
			texture->getImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.setIndex( MinTextureIndex );
			unit.initialise();
			return unit;
		}

		FrameBufferSPtr doCreateFbo( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto fbo = renderSystem.createFrameBuffer();
			fbo->initialise();
			return fbo;
		}

		AttachmentPoint doGetAttachmentPoint( PixelFormat format )
		{
			return format == PixelFormat::eD16
					|| format == PixelFormat::eD24
					|| format == PixelFormat::eD24S8
					|| format == PixelFormat::eD32
					|| format == PixelFormat::eD32F
					|| format == PixelFormat::eD32FS8
				? AttachmentPoint::eDepth
				: AttachmentPoint::eColour;
		}

		TextureAttachmentSPtr doCreateAttach( FrameBuffer & fbo
			, TextureLayoutSPtr texture
			, AttachmentPoint point )
		{
			auto attach = fbo.createAttachment( texture );
			fbo.bind();
			fbo.attach( point, 0u, attach, texture->getType() );
			fbo.setDrawBuffer( attach );
			ENSURE( fbo.isComplete() );
			fbo.unbind();
			return attach;
		}
	}

	//*********************************************************************************************

	String const GaussianBlur::Config = cuT( "Config" );
	String const GaussianBlur::Coefficients = cuT( "c3d_coefficients" );
	String const GaussianBlur::CoefficientsCount = cuT( "c3d_coefficientsCount" );
	String const GaussianBlur::TextureSize = cuT( "c3d_textureSize" );

	GaussianBlur::GaussianBlur( Engine & engine
		, Size const & textureSize
		, PixelFormat format
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ engine }
		, m_size{ textureSize }
		, m_matrixUbo{ engine }
		, m_point{ doGetAttachmentPoint( format ) }
		, m_colour{ doCreateTexture( engine, textureSize, format, cuT( "GaussianBlur" ) ) }
		, m_fbo{ doCreateFbo( engine, textureSize ) }
		, m_colourAttach{ doCreateAttach( *m_fbo, m_colour.getTexture(), m_point ) }
		, m_kernel{ getHalfPascal( kernelSize ) }
		, m_blurUbo{ GaussianBlur::Config, *engine.getRenderSystem(), 2u }
		, m_blurCoeffCount{ m_blurUbo.createUniform< UniformType::eUInt >( GaussianBlur::CoefficientsCount ) }
		, m_blurCoeffs{ m_blurUbo.createUniform< UniformType::eFloat >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients ) }
		, m_blurSize{ m_blurUbo.createUniform< UniformType::eVec2f >( GaussianBlur::TextureSize ) }
	{
		m_blurCoeffCount->setValue( uint32_t( m_kernel.size() ) );
		m_blurCoeffs->setValues( m_kernel );
		doInitialiseBlurXProgram( engine );
		doInitialiseBlurYProgram( engine );
	}

	GaussianBlur::~GaussianBlur()
	{
		m_blurYPipeline->cleanup();
		m_blurYPipeline.reset();
		m_blurXPipeline->cleanup();
		m_blurXPipeline.reset();

		m_blurUbo.cleanup();
		m_blurCoeffCount.reset();
		m_blurCoeffs.reset();
		m_blurSize.reset();
		m_matrixUbo.getUbo().cleanup();

		m_fbo->bind();
		m_fbo->detachAll();
		m_fbo->unbind();
		m_fbo->cleanup();
		m_fbo.reset();
		m_colourAttach.reset();
		m_colour.cleanup();
	}

	void GaussianBlur::blur( TextureLayoutSPtr texture )
	{
		REQUIRE( texture->getType() == m_colour.getTexture()->getType() );
		REQUIRE( texture->getPixelFormat() == m_colour.getTexture()->getPixelFormat() );
		doBlur( texture
			, m_colour.getTexture()
			, m_colourAttach
			, *m_blurXPipeline );
		auto attach = doCreateAttach( *m_fbo
			, texture
			, doGetAttachmentPoint( texture->getPixelFormat() ) );
		doBlur( m_colour.getTexture()
			, texture
			, attach
			, *m_blurYPipeline );
	}

	void GaussianBlur::blur( TextureLayoutSPtr texture
		, TextureLayoutSPtr intermediate )
	{
		REQUIRE( texture->getType() == intermediate->getType() );
		REQUIRE( texture->getPixelFormat() == intermediate->getPixelFormat() );
		auto attachDst = doCreateAttach( *m_fbo
			, intermediate
			, doGetAttachmentPoint( intermediate->getPixelFormat() ) );
		doBlur( texture
			, intermediate
			, attachDst
			, *m_blurXPipeline );
		auto attachSrc = doCreateAttach( *m_fbo
			, texture
			, doGetAttachmentPoint( texture->getPixelFormat() ) );
		doBlur( intermediate
			, texture
			, attachSrc
			, *m_blurYPipeline );
	}

	void GaussianBlur::doBlur( TextureLayoutSPtr source
		, TextureLayoutSPtr destination
		, TextureAttachmentSPtr attach
		, RenderPipeline & pipeline )
	{
		auto context = getEngine()->getRenderSystem()->getCurrentContext();
		m_blurSize->setValue( Point2f{ source->getDimensions().getWidth()
			, source->getDimensions().getHeight() } );
		m_blurUbo.update();
		m_blurUbo.bindTo( 2u );
		m_fbo->bind( FrameBufferTarget::eDraw );
		attach->setLayer( 0 );
		attach->attach( m_point, 0u );
		m_fbo->setDrawBuffer( attach );
		m_fbo->clear( BufferComponent::eColour );
		context->renderTexture( source->getDimensions()
			, *source
			, pipeline
			, m_matrixUbo );
		m_fbo->unbind();
	}

	bool GaussianBlur::doInitialiseBlurXProgram( Engine & engine )
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & cache = engine.getShaderProgramCache();
		auto const vertex = getVertexProgram( engine );
		auto const blurX = getBlurXProgram( engine );

		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vertex );
		program->setSource( ShaderType::ePixel, blurX );
		bool result = program->initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			m_blurXPipeline = renderSystem.createRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_blurXPipeline->addUniformBuffer( m_matrixUbo.getUbo() );
			m_blurXPipeline->addUniformBuffer( m_blurUbo );
		}

		return result;
	}

	bool GaussianBlur::doInitialiseBlurYProgram( Engine & engine )
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & cache = engine.getShaderProgramCache();
		auto const vertex = getVertexProgram( engine );
		auto const blurY = getBlurYProgram( engine );

		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vertex );
		program->setSource( ShaderType::ePixel, blurY );
		bool result = program->initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			m_blurYPipeline = renderSystem.createRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_blurYPipeline->addUniformBuffer( m_matrixUbo.getUbo() );
			m_blurYPipeline->addUniformBuffer( m_blurUbo );
		}

		return result;
	}
}
