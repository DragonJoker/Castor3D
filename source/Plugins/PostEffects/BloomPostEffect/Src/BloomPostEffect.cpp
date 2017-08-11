#include "BloomPostEffect.hpp"

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

using namespace castor;
using namespace castor3d;

namespace Bloom
{
	namespace
	{
		glsl::Shader getVertexProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getHiPassProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_v4FragColor = writer.declFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_v4FragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
				auto maxComponent = writer.declLocale( cuT( "maxComponent" ), glsl::max( pxl_v4FragColor.r(), pxl_v4FragColor.g() ) );
				maxComponent = glsl::max( maxComponent, pxl_v4FragColor.b() );

				IF( writer, maxComponent > 1.0_f )
				{
					pxl_v4FragColor.xyz() /= maxComponent;
				}
				ELSE
				{
					pxl_v4FragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );
			return writer.finalise();
		}

		glsl::Shader getCombineProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapPass0 = writer.declUniform< Sampler2D >( BloomPostEffect::CombineMapPass0 );
			auto c3d_mapPass1 = writer.declUniform< Sampler2D >( BloomPostEffect::CombineMapPass1 );
			auto c3d_mapPass2 = writer.declUniform< Sampler2D >( BloomPostEffect::CombineMapPass2 );
			auto c3d_mapPass3 = writer.declUniform< Sampler2D >( BloomPostEffect::CombineMapPass3 );
			auto c3d_mapScene = writer.declUniform< Sampler2D >( BloomPostEffect::CombineMapScene );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = texture( c3d_mapPass0, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass1, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass2, vtx_texture );
				plx_v4FragColor += texture( c3d_mapPass3, vtx_texture );
				plx_v4FragColor += texture( c3d_mapScene, vtx_texture );
			} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	String const BloomPostEffect::Type = cuT( "bloom" );
	String const BloomPostEffect::Name = cuT( "Bloom PostEffect" );
	String const BloomPostEffect::CombineMapPass0 = cuT( "c3d_mapPass0" );
	String const BloomPostEffect::CombineMapPass1 = cuT( "c3d_mapPass1" );
	String const BloomPostEffect::CombineMapPass2 = cuT( "c3d_mapPass2" );
	String const BloomPostEffect::CombineMapPass3 = cuT( "c3d_mapPass3" );
	String const BloomPostEffect::CombineMapScene = cuT( "c3d_mapScene" );

	BloomPostEffect::BloomPostEffect( RenderTarget & p_renderTarget
		, RenderSystem & renderSystem
		, Parameters const & p_param )
		: PostEffect( BloomPostEffect::Type
			, p_renderTarget
			, renderSystem
			, p_param )
		, m_viewport{ *renderSystem.getEngine() }
		, m_size( 5u )
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_declaration(
		{
			BufferElementDeclaration( ShaderProgram::Position
				, uint32_t( ElementUsage::ePosition )
				, ElementType::eVec2 ),
		} )
		, m_hiPassSurfaces(
		{
			{
				PostEffectSurface{ *renderSystem.getEngine() },
				PostEffectSurface{ *renderSystem.getEngine() },
				PostEffectSurface{ *renderSystem.getEngine() },
				PostEffectSurface{ *renderSystem.getEngine() }
			}
		} )
		, m_blurSurfaces(
		{
			{
				PostEffectSurface{ *renderSystem.getEngine() },
				PostEffectSurface{ *renderSystem.getEngine() },
				PostEffectSurface{ *renderSystem.getEngine() },
				PostEffectSurface{ *renderSystem.getEngine() }
			}
		} )
	{
		String count;

		if ( p_param.get( cuT( "Size" ), count ) )
		{
			m_size = uint32_t( string::toLong( count ) );
		}

		real pBuffer[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		std::memcpy( m_buffer, pBuffer, sizeof( pBuffer ) );
		uint32_t i = 0;

		for ( auto & vertex : m_vertices )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_buffer )[i++ * m_declaration.stride()] );
		}

		m_linearSampler = doCreateSampler( true );
		m_nearestSampler = doCreateSampler( false );
	}

	BloomPostEffect::~BloomPostEffect()
	{
	}

	PostEffectSPtr BloomPostEffect::create( RenderTarget & p_renderTarget
		, RenderSystem & renderSystem
		, Parameters const & p_param )
	{
		return std::make_shared< BloomPostEffect >( p_renderTarget
			, renderSystem
			, p_param );
	}

	bool BloomPostEffect::initialise()
	{
		bool result = false;
		m_viewport.initialise();

		result = doInitialiseHiPassProgram();
		Size size = m_renderTarget.getSize();

		if ( result )
		{
			result = doInitialiseCombineProgram();
		}

		uint32_t index = 0;

		for ( auto & surface : m_hiPassSurfaces )
		{
			surface.initialise( m_renderTarget
				, size
				, index++
				, m_linearSampler );
			size.getWidth() >>= 1;
			size.getHeight() >>= 1;
		}

		size = m_renderTarget.getSize();
		index = 0;

		for ( auto & surface : m_blurSurfaces )
		{
			surface.initialise( m_renderTarget
				, size
				, index++
				, m_nearestSampler );
			size.getWidth() >>= 1;
			size.getHeight() >>= 1;
		}

		if ( result )
		{
			m_blur = std::make_shared< GaussianBlur >( *getRenderSystem()->getEngine()
				, size
				, m_hiPassSurfaces[0].m_colourTexture.getTexture()->getPixelFormat()
				, m_size );
		}

		return result;
	}

	void BloomPostEffect::cleanup()
	{
		m_matrixUbo.getUbo().cleanup();
		m_blur.reset();
		m_hiPassMapDiffuse.reset();

		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();

		for ( auto & surface : m_blurSurfaces )
		{
			surface.cleanup();
		}

		for ( auto & surface : m_hiPassSurfaces )
		{
			surface.cleanup();
		}

		m_viewport.cleanup();

		m_combinePipeline->cleanup();
		m_combinePipeline.reset();

		m_hiPassPipeline->cleanup();
		m_hiPassPipeline.reset();
	}

	bool BloomPostEffect::apply( FrameBuffer & p_framebuffer )
	{
		auto attach = p_framebuffer.getAttachment( AttachmentPoint::eColour, 0 );

		if ( attach && attach->getAttachmentType() == AttachmentType::eTexture )
		{
			auto const & texture = *std::static_pointer_cast< TextureAttachment >( attach )->getTexture();

			doHiPassFilter( texture );
			doDownSample( texture );

			for ( uint32_t i = 1; i < m_hiPassSurfaces.size(); ++i )
			{
				m_blur->blur( m_hiPassSurfaces[i].m_colourTexture.getTexture()
					, m_blurSurfaces[i].m_colourTexture.getTexture() );
			}

#if 0

			for ( auto & surface : m_hiPassSurfaces )
			{
				auto pxbuffer = surface.m_colourTexture.getTexture()->getImage().getBuffer();

				if ( pxbuffer )
				{
					if ( surface.m_colourTexture.getTexture()->bind( 0 ) )
					{
						auto buffer = surface.m_colourTexture.getTexture()->lock( AccessType::eRead );

						if ( buffer )
						{
							std::memcpy( pxbuffer->ptr(), buffer, pxbuffer->size() );
							surface.m_colourTexture.getTexture()->unlock( false );
							StringStream name;
							name << Engine::getEngineDirectory() << cuT( "\\Bloom_" ) << ( void * )pxbuffer.get() << cuT( "_Blur.png" );
							Image::BinaryWriter()( Image( cuT( "tmp" ), *pxbuffer ), Path( name.str() ) );
						}

						surface.m_colourTexture.getTexture()->unbind( 0 );
					}
				}
			}

#endif
			doCombine( texture );

			p_framebuffer.bind( FrameBufferTarget::eDraw );
			getRenderSystem()->getCurrentContext()->renderTexture( texture.getDimensions()
				, *m_blurSurfaces[0].m_colourTexture.getTexture() );
			p_framebuffer.unbind();
		}

		return true;
	}

	bool BloomPostEffect::doWriteInto( TextFile & p_file )
	{
		return p_file.writeText( cuT( " -Size=" ) + string::toString( m_size ) ) > 0;
	}

	void BloomPostEffect::doHiPassFilter( TextureLayout const & p_origin )
	{
		auto source = &m_hiPassSurfaces[0];
		source->m_fbo->bind( FrameBufferTarget::eDraw );
		source->m_fbo->clear( BufferComponent::eColour );
		m_hiPassMapDiffuse->setValue( 0 );
		getRenderSystem()->getCurrentContext()->renderTexture( source->m_size
			, p_origin
			, *m_hiPassPipeline
			, m_matrixUbo );
		source->m_fbo->unbind();
	}

	void BloomPostEffect::doDownSample( TextureLayout const & p_origin )
	{
		auto context = getRenderSystem()->getCurrentContext();
		auto source = &m_hiPassSurfaces[0];

		for ( uint32_t i = 1; i < m_hiPassSurfaces.size(); ++i )
		{
			auto destination = &m_hiPassSurfaces[i];
			destination->m_fbo->bind( FrameBufferTarget::eDraw );
			destination->m_fbo->clear( BufferComponent::eColour );
			context->renderTexture( destination->m_size
				, *source->m_colourTexture.getTexture() );
			destination->m_fbo->unbind();
			source = destination;
		}
	}

	void BloomPostEffect::doCombine( TextureLayout const & p_origin )
	{
		m_blurSurfaces[0].m_fbo->bind( FrameBufferTarget::eDraw );
		m_blurSurfaces[0].m_fbo->clear( BufferComponent::eColour );
		m_viewport.resize( p_origin.getDimensions() );
		m_viewport.update();
		m_viewport.apply();

		auto const & texture0 = m_hiPassSurfaces[0].m_colourTexture;
		auto const & texture1 = m_hiPassSurfaces[1].m_colourTexture;
		auto const & texture2 = m_hiPassSurfaces[2].m_colourTexture;
		auto const & texture3 = m_hiPassSurfaces[3].m_colourTexture;
		m_matrixUbo.update( m_viewport.getProjection() );
		m_combinePipeline->apply();

		texture0.bind();
		texture1.bind();
		texture2.bind();
		texture3.bind();
		p_origin.bind( 4 );
		m_linearSampler->bind( 4 );

		m_geometryBuffers->draw( uint32_t( m_vertices.size() ), 0 );

		texture0.unbind();
		texture1.unbind();
		texture2.unbind();
		texture3.unbind();
		p_origin.unbind( 4 );
		m_linearSampler->unbind( 4 );

		m_blurSurfaces[0].m_fbo->unbind();

		m_blurSurfaces[0].m_colourTexture.bind();
		m_blurSurfaces[0].m_colourTexture.getTexture()->generateMipmaps();
		m_blurSurfaces[0].m_colourTexture.unbind();
	}

	SamplerSPtr BloomPostEffect::doCreateSampler( bool p_linear )
	{
		String name = cuT( "BloomSampler_" );
		InterpolationMode mode;

		if ( p_linear )
		{
			name += cuT( "Linear" );
			mode = InterpolationMode::eLinear;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = InterpolationMode::eNearest;
		}

		SamplerSPtr sampler;

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			sampler->setInterpolationMode( InterpolationFilter::eMin, mode );
			sampler->setInterpolationMode( InterpolationFilter::eMag, mode );
			sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		}
		else
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		return sampler;
	}

	bool BloomPostEffect::doInitialiseHiPassProgram()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const hipass = getHiPassProgram( getRenderSystem() );

		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		m_hiPassMapDiffuse = program->createUniform < UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vertex );
		program->setSource( ShaderType::ePixel, hipass );
		bool result = program->initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			m_hiPassPipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_hiPassPipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		}

		return result;
	}

	bool BloomPostEffect::doInitialiseCombineProgram()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const combine = getCombineProgram( getRenderSystem() );

		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass0
			, ShaderType::ePixel )->setValue( 0 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass1
			, ShaderType::ePixel )->setValue( 1 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass2
			, ShaderType::ePixel )->setValue( 2 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass3
			, ShaderType::ePixel )->setValue( 3 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapScene
			, ShaderType::ePixel )->setValue( 4 );

		program->setSource( ShaderType::eVertex, vertex );
		program->setSource( ShaderType::ePixel, combine );
		bool result = program->initialise();

		if ( result )
		{
			m_vertexBuffer = std::make_shared< VertexBuffer >( *getRenderSystem()->getEngine()
				, m_declaration );
			m_vertexBuffer->resize( uint32_t( m_vertices.size() * m_declaration.stride() ) );
			m_vertexBuffer->linkCoords( m_vertices.begin(), m_vertices.end() );
			m_vertexBuffer->initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
			m_geometryBuffers = getRenderSystem()->createGeometryBuffers( Topology::eTriangles
				, *program );
			result = m_geometryBuffers->initialise( { *m_vertexBuffer }, nullptr );
		}

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			m_combinePipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_combinePipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		}

		return result;
	}
}
