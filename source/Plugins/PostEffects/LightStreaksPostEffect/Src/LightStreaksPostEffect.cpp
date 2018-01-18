#include "LightStreaksPostEffect.hpp"

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
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <GlslSource.hpp>

namespace light_streaks
{
	namespace
	{
		glsl::Shader getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			UBO_MATRIX( writer, 0u );

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( castor3d::ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_projection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getHiPassProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( castor3d::ShaderProgram::MapDiffuse, castor3d::MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
				auto norm = writer.declLocale( cuT( "norm" )
					, length( pxl_fragColor.rgb() ) );

				IF ( writer, norm < 0.5_f )
				{
					pxl_fragColor.xyz() = vec3( 0.0_f );
				}
				ELSE
				{
					pxl_fragColor.xyz() *= 1.0_f / norm;
				}
				FI;
			} );
			return writer.finalise();
		}

		glsl::Shader getKawaseProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( castor3d::ShaderProgram::MapDiffuse, castor3d::MinTextureIndex, 0u );
			UBO_KAWASE( writer );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" )
					, vec3( 0.0_f ) );
				auto b = writer.declLocale( cuT( "b" )
					, pow( writer.cast< Float >( c3d_samples ), writer.cast< Float >( c3d_pass ) ) );

				FOR( writer, Int, s, 0, "s < c3d_samples", "s++" )
				{
					// Weight = a^(b*s)
					auto weight = writer.declLocale( cuT( "weight" )
						, pow( c3d_attenuation, b * writer.cast< Float >( s ) ) );
					// Streak direction is a 2D vector in image space
					auto sampleCoord = writer.declLocale( cuT( "sampleCoord" )
						, vtx_texture + ( c3d_direction * b * vec2( s, s ) * c3d_pixelSize ) );
					// Scale and accumulate
					colour += texture( c3d_mapDiffuse, sampleCoord ).rgb() * clamp( weight, 0.0_f, 1.0_f );
				}
				ROF;

				pxl_fragColor.rgb() = clamp( colour, vec3( 0.0_f ), vec3( 1.0_f ) );
				pxl_fragColor.a() = 1.0_f;
			} );
			return writer.finalise();
		}

		glsl::Shader getCombineProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto index = castor3d::MinTextureIndex;
			auto c3d_mapScene = writer.declSampler< Sampler2D >( PostEffect::CombineMapScene, index++, 0u );
			auto c3d_mapKawase1 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase1, index++, 0u );
			auto c3d_mapKawase2 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase2, index++, 0u );
			auto c3d_mapKawase3 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase3, index++, 0u );
			auto c3d_mapKawase4 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase4, index++, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = texture( c3d_mapScene, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase1, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase2, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase3, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase4, vtx_texture );
				pxl_fragColor.a() = 1.0_f;
			} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	castor::String const PostEffect::Type = cuT( "light_streaks" );
	castor::String const PostEffect::Name = cuT( "LightStreaks PostEffect" );
	castor::String const PostEffect::CombineMapScene = cuT( "c3d_mapScene" );
	castor::String const PostEffect::CombineMapKawase1 = cuT( "c3d_mapKawase1" );
	castor::String const PostEffect::CombineMapKawase2 = cuT( "c3d_mapKawase2" );
	castor::String const PostEffect::CombineMapKawase3 = cuT( "c3d_mapKawase3" );
	castor::String const PostEffect::CombineMapKawase4 = cuT( "c3d_mapKawase4" );

	PostEffect::PostEffect( castor3d::RenderTarget & p_renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & p_param )
		: castor3d::PostEffect( PostEffect::Type
			, p_renderTarget
			, renderSystem
			, p_param )
		, m_viewport{ *renderSystem.getEngine() }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_kawaseUbo{ *renderSystem.getEngine() }
		, m_declaration(
		{
			castor3d::BufferElementDeclaration( castor3d::ShaderProgram::Position
				, uint32_t( castor3d::ElementUsage::ePosition )
				, castor3d::renderer::AttributeFormat::eVec2 ),
		} )
		, m_hiPassSurfaces
		{
			PostEffectSurface{ *renderSystem.getEngine() },
			PostEffectSurface{ *renderSystem.getEngine() },
			PostEffectSurface{ *renderSystem.getEngine() },
			PostEffectSurface{ *renderSystem.getEngine() },
		}
		, m_kawaseSurfaces
		{
			PostEffectSurface{ *renderSystem.getEngine() },
			PostEffectSurface{ *renderSystem.getEngine() },
			PostEffectSurface{ *renderSystem.getEngine() },
			PostEffectSurface{ *renderSystem.getEngine() },
		}
		, m_combineSurface{ *renderSystem.getEngine() }
	{
		castor::real buffer[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		std::memcpy( m_buffer, buffer, sizeof( buffer ) );
		uint32_t i = 0;

		for ( auto & vertex : m_vertices )
		{
			vertex = std::make_shared< castor3d::BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_buffer )[i++ * m_declaration.stride()] );
		}

		m_linearSampler = doCreateSampler( true );
		m_nearestSampler = doCreateSampler( false );
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & p_renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & p_param )
	{
		return std::make_shared< PostEffect >( p_renderTarget
			, renderSystem
			, p_param );
	}

	bool PostEffect::initialise()
	{
		bool result = doInitialiseHiPassProgram();

		if ( result )
		{
			result = doInitialiseKawaseProgram();
		}

		if ( result )
		{
			result = doInitialiseCombineProgram();
		}

		castor::Size size = m_renderTarget.getSize();
		m_viewport.resize( size );
		m_viewport.initialise();
		m_viewport.update();
		m_combineSurface.initialise( m_renderTarget
			, size
			, castor3d::MinTextureIndex
			, m_linearSampler );

		size.getWidth() >>= 2;
		size.getHeight() >>= 2;

		for ( uint32_t i = 0u; i < Count; ++i )
		{
			m_hiPassSurfaces[i].initialise( m_renderTarget
				, size
				, castor3d::MinTextureIndex
				, m_linearSampler );
			m_kawaseSurfaces[i].initialise( m_renderTarget
				, size
				, castor3d::MinTextureIndex + 1u + i
				, m_linearSampler );
		}

		return result;
	}

	void PostEffect::cleanup()
	{
		m_matrixUbo.getUbo().cleanup();
		m_kawaseUbo.getUbo().cleanup();

		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();

		for ( size_t i = 0u; i < Count; ++i )
		{
			m_hiPassSurfaces[i].cleanup();
			m_kawaseSurfaces[i].cleanup();
		}

		m_combineSurface.cleanup();

		m_viewport.cleanup();

		m_combinePipeline->cleanup();
		m_combinePipeline.reset();

		m_kawasePipeline->cleanup();
		m_kawasePipeline.reset();

		m_hiPassPipeline->cleanup();
		m_hiPassPipeline.reset();
	}

	bool PostEffect::apply( castor3d::FrameBuffer & framebuffer )
	{
		auto attach = framebuffer.getAttachment( castor3d::AttachmentPoint::eColour, 0 );

		if ( attach && attach->getAttachmentType() == castor3d::AttachmentType::eTexture )
		{
			auto const & texture = *std::static_pointer_cast< castor3d::TextureAttachment >( attach )->getTexture();

			doHiPassFilter( texture );
			doKawaseBlur( texture, framebuffer );
			doCombine( texture );

			framebuffer.bind( castor3d::FrameBufferTarget::eDraw );
			getRenderSystem()->getCurrentContext()->renderTexture( texture.getDimensions()
				, *m_combineSurface.m_colourTexture.getTexture() );
			framebuffer.unbind();
		}

		return true;
	}

	bool PostEffect::doWriteInto( castor::TextFile & file )
	{
		return true;
	}

	void PostEffect::doHiPassFilter( castor3d::TextureLayout const & origin )
	{
		for ( auto & destination : m_hiPassSurfaces )
		{
			destination.m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
			destination.m_fbo->clear( castor3d::BufferComponent::eColour );
			getRenderSystem()->getCurrentContext()->renderTexture( destination.m_size
				, origin
				, *m_hiPassPipeline
				, m_matrixUbo );
			destination.m_fbo->unbind();
		}
	}

	void PostEffect::doKawaseBlur( castor3d::TextureLayout const & origin
		, castor3d::FrameBuffer & framebuffer )
	{
		static float constexpr factor = 0.2f;
		static std::array< castor::Point2f, Count > directions
		{
			{
				castor::Point2f{ factor, factor },
				castor::Point2f{ -factor, -factor },
				castor::Point2f{ -factor, factor },
				castor::Point2f{ factor, -factor }
			}
		};

		auto context = getRenderSystem()->getCurrentContext();

		for ( size_t j = 0u; j < Count; ++j )
		{
			auto source = &m_hiPassSurfaces[j];
			auto destination = &m_kawaseSurfaces[j];
			auto & direction = directions[j];

			for ( uint32_t i = 0u; i < 3u; ++i )
			{
				destination->m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
				destination->m_fbo->clear( castor3d::BufferComponent::eColour );
				m_kawaseUbo.update( source->m_size
					, direction
					, i );
				context->renderTexture( destination->m_size
					, *source->m_colourTexture.getTexture()
					, *m_kawasePipeline
					, m_matrixUbo );
				destination->m_fbo->unbind();
				std::swap( destination, source );
			}
		}
	}

	void PostEffect::doCombine( castor3d::TextureLayout const & p_origin )
	{
		auto destination = &m_combineSurface;
		destination->m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
		destination->m_fbo->clear( castor3d::BufferComponent::eColour );
		m_viewport.apply();
		m_matrixUbo.update( m_viewport.getProjection() );

		auto const & kawase1 = m_kawaseSurfaces[0].m_colourTexture;
		auto const & kawase2 = m_kawaseSurfaces[1].m_colourTexture;
		auto const & kawase3 = m_kawaseSurfaces[2].m_colourTexture;
		auto const & kawase4 = m_kawaseSurfaces[3].m_colourTexture;
		m_combinePipeline->apply();

		p_origin.bind( castor3d::MinTextureIndex );
		m_linearSampler->bind( castor3d::MinTextureIndex );
		kawase1.bind();
		kawase2.bind();
		kawase3.bind();
		kawase4.bind();

		m_geometryBuffers->draw( uint32_t( m_vertices.size() ), 0u );

		kawase4.unbind();
		kawase3.unbind();
		kawase2.unbind();
		kawase1.unbind();
		m_linearSampler->unbind( castor3d::MinTextureIndex );
		p_origin.unbind( castor3d::MinTextureIndex );

		destination->m_fbo->unbind();

		destination->m_colourTexture.bind();
		destination->m_colourTexture.getTexture()->generateMipmaps();
		destination->m_colourTexture.unbind();
	}

	castor3d::SamplerSPtr PostEffect::doCreateSampler( bool p_linear )
	{
		castor::String name = cuT( "FlareSampler_" );
		castor3d::InterpolationMode mode;

		if ( p_linear )
		{
			name += cuT( "Linear" );
			mode = castor3d::InterpolationMode::eLinear;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = castor3d::InterpolationMode::eNearest;
		}

		castor3d::SamplerSPtr sampler;

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			sampler->setInterpolationMode( castor3d::InterpolationFilter::eMin, mode );
			sampler->setInterpolationMode( castor3d::InterpolationFilter::eMag, mode );
			sampler->setWrappingMode( castor3d::TextureUVW::eU, castor3d::WrapMode::eClampToEdge );
			sampler->setWrappingMode( castor3d::TextureUVW::eV, castor3d::WrapMode::eClampToEdge );
			sampler->setWrappingMode( castor3d::TextureUVW::eW, castor3d::WrapMode::eClampToEdge );
		}
		else
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		return sampler;
	}

	bool PostEffect::doInitialiseHiPassProgram()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const hipass = getHiPassProgram( getRenderSystem() );

		castor3d::ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, hipass );
		bool result = program->initialise();

		if ( result )
		{
			castor3d::DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( castor3d::WritingMask::eZero );
			m_hiPassPipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, castor3d::RasteriserState{}
				, castor3d::BlendState{}
				, castor3d::MultisampleState{}
				, *program
				, castor3d::PipelineFlags{} );
			m_hiPassPipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		}

		return result;
	}

	bool PostEffect::doInitialiseKawaseProgram()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const hipass = getKawaseProgram( getRenderSystem() );

		castor3d::ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, hipass );
		bool result = program->initialise();

		if ( result )
		{
			castor3d::DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( castor3d::WritingMask::eZero );
			m_kawasePipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, castor3d::RasteriserState{}
				, castor3d::BlendState{}
				, castor3d::MultisampleState{}
				, *program
				, castor3d::PipelineFlags{} );
			m_kawasePipeline->addUniformBuffer( m_matrixUbo.getUbo() );
			m_kawasePipeline->addUniformBuffer( m_kawaseUbo.getUbo() );
		}

		return result;
	}

	bool PostEffect::doInitialiseCombineProgram()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const combine = getCombineProgram( getRenderSystem() );

		castor3d::ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, combine );
		bool result = program->initialise();

		if ( result )
		{
			m_vertexBuffer = std::make_shared< castor3d::VertexBuffer >( *getRenderSystem()->getEngine()
				, m_declaration );
			m_vertexBuffer->resize( uint32_t( m_vertices.size() * m_declaration.stride() ) );
			m_vertexBuffer->linkCoords( m_vertices.begin(), m_vertices.end() );
			m_vertexBuffer->initialise( renderer::MemoryPropertyFlag::eHostVisible );
			m_geometryBuffers = getRenderSystem()->createGeometryBuffers( castor3d::Topology::eTriangles
				, *program );
			result = m_geometryBuffers->initialise( { *m_vertexBuffer }, nullptr );
		}

		if ( result )
		{
			castor3d::DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( castor3d::WritingMask::eZero );
			m_combinePipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, castor3d::RasteriserState{}
				, castor3d::BlendState{}
				, castor3d::MultisampleState{}
				, *program
				, castor3d::PipelineFlags{} );
			m_combinePipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		}

		return result;
	}
}
