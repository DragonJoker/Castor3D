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
#include "Shader/Uniform.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		VertexBufferSPtr doCreateVbo( Engine & engine )
		{
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
			} );

			float data[] =
			{
				0, 0,
				1, 1,
				0, 1,
				0, 0,
				1, 0,
				1, 1
			};

			auto & renderSystem = *engine.getRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		glsl::Shader getVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = position;
					gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
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
			config.end();
			auto c3d_mapDiffuse = writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );
			auto gl_FragDepth = writer.declBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 1.0_f, 0 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				plx_v4FragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, Int, i, 1, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = plx_v4FragColor.r();
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
			config.end();
			auto c3d_mapDiffuse = writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );
			auto gl_FragDepth = writer.declBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 0.0_f, 1 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				plx_v4FragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, Int, i, 1, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = plx_v4FragColor.r();
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
			unit.setIndex( 0u );
			unit.initialise();
			return unit;
		}

		FrameBufferSPtr doCreateFbo( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto fbo = renderSystem.createFrameBuffer();
			fbo->create();
			fbo->initialise( size );
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
			, TextureUnit const & unit
			, AttachmentPoint point )
		{
			auto attach = fbo.createAttachment( unit.getTexture() );
			fbo.bind();
			fbo.attach( point, 0u, attach, unit.getTexture()->getType() );
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
		, m_colourAttach{ doCreateAttach( *m_fbo, m_colour, m_point ) }
		, m_kernel{ getHalfPascal( kernelSize ) }
		, m_blurXUbo{ GaussianBlur::Config, *engine.getRenderSystem(), 2u }
		, m_blurYUbo{ GaussianBlur::Config, *engine.getRenderSystem(), 2u }
		, m_blurXCoeffCount{ m_blurXUbo.createUniform< UniformType::eUInt >( GaussianBlur::CoefficientsCount ) }
		, m_blurXCoeffs{ m_blurXUbo.createUniform< UniformType::eFloat >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients ) }
		, m_blurYCoeffCount{ m_blurYUbo.createUniform< UniformType::eUInt >( GaussianBlur::CoefficientsCount ) }
		, m_blurYCoeffs{ m_blurYUbo.createUniform< UniformType::eFloat >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients ) }
	{
		doInitialiseBlurXProgram( engine );
		doInitialiseBlurYProgram( engine );
	}

	GaussianBlur::~GaussianBlur()
	{
		m_blurXUbo.cleanup();
		m_blurXCoeffCount.reset();
		m_blurXCoeffs.reset();
		m_blurXMapDiffuse.reset();
		m_blurYPipeline->cleanup();
		m_blurYPipeline.reset();

		m_blurYUbo.cleanup();
		m_blurYCoeffCount.reset();
		m_blurYCoeffs.reset();
		m_blurYMapDiffuse.reset();
		m_blurXPipeline->cleanup();
		m_blurXPipeline.reset();

		m_matrixUbo.getUbo().cleanup();
		m_fbo->bind();
		m_fbo->detachAll();
		m_fbo->unbind();
		m_fbo->cleanup();
		m_fbo->destroy();
		m_fbo.reset();
		m_colourAttach.reset();
		m_colour.cleanup();
	}

	void GaussianBlur::blur( TextureLayoutSPtr texture )
	{
		blur( texture, m_colour.getTexture() );
	}

	void GaussianBlur::blur( TextureLayoutSPtr texture
		, TextureLayoutSPtr intermediate )
	{
		REQUIRE( texture->getPixelFormat() == intermediate->getPixelFormat() );
		doBlur( texture, intermediate, *m_blurXPipeline, m_blurXUbo );
		doBlur( intermediate, texture, *m_blurYPipeline, m_blurYUbo );
	}

	void GaussianBlur::doBlur( TextureLayoutSPtr & source
		, TextureLayoutSPtr & destination
		, RenderPipeline & pipeline
		, UniformBuffer & ubo )
	{
		auto context = getEngine()->getRenderSystem()->getCurrentContext();
		ubo.bindTo( 2u );
		m_fbo->bind( FrameBufferTarget::eDraw );
		auto attach = m_fbo->createAttachment( destination );
		attach->setLayer( 0u );
		attach->setTarget( TextureType::eTwoDimensions );
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
		m_blurXMapDiffuse = program->createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		m_blurXCoeffCount->setValue( uint32_t( m_kernel.size() ) );
		m_blurXCoeffs->setValues( m_kernel );

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
			m_blurXPipeline->addUniformBuffer( m_blurXUbo );
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
		m_blurYMapDiffuse = program->createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		m_blurYCoeffCount->setValue( uint32_t( m_kernel.size() ) );
		m_blurYCoeffs->setValues( m_kernel );

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
			m_blurYPipeline->addUniformBuffer( m_blurYUbo );
		}

		return result;
	}
}
