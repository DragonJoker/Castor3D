#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <FrameBuffer/BackBuffers.hpp>
#include <FrameBuffer/FrameBufferAttachment.hpp>
#include <FrameBuffer/FrameBufferAttachment.hpp>
#include <Mesh/Vertex.hpp>
#include <Buffer/ParticleDeclaration.hpp>
#include <Buffer/ParticleElementDeclaration.hpp>
#include <Buffer/GeometryBuffers.hpp>
#include <Buffer/VertexBuffer.hpp>
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

			UBO_MATRIX( writer, 0u );

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ) );

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

		glsl::Shader getHiPassProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
				auto maxComponent = writer.declLocale( cuT( "maxComponent" ), glsl::max( pxl_fragColor.r(), pxl_fragColor.g() ) );
				maxComponent = glsl::max( maxComponent, pxl_fragColor.b() );

				IF( writer, maxComponent > 1.0_f )
				{
					pxl_fragColor.xyz() /= maxComponent;
				}
				ELSE
				{
					pxl_fragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
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
			auto index = MinTextureIndex;
			auto c3d_mapPass0 = writer.declSampler< Sampler2D >( BloomPostEffect::CombineMapPass0, index++, 0u );
			auto c3d_mapPass1 = writer.declSampler< Sampler2D >( BloomPostEffect::CombineMapPass1, index++, 0u );
			auto c3d_mapPass2 = writer.declSampler< Sampler2D >( BloomPostEffect::CombineMapPass2, index++, 0u );
			auto c3d_mapPass3 = writer.declSampler< Sampler2D >( BloomPostEffect::CombineMapPass3, index++, 0u );
			auto c3d_mapScene = writer.declSampler< Sampler2D >( BloomPostEffect::CombineMapScene, index++, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = texture( c3d_mapPass0, vtx_texture );
				pxl_fragColor += texture( c3d_mapPass1, vtx_texture );
				pxl_fragColor += texture( c3d_mapPass2, vtx_texture );
				pxl_fragColor += texture( c3d_mapPass3, vtx_texture );
				pxl_fragColor += texture( c3d_mapScene, vtx_texture );
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
			ParticleElementDeclaration( cuT( "position" )
				, uint32_t( ElementUsage::ePosition )
				, renderer::AttributeFormat::eVec2 ),
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
		, m_combineSurface{ *renderSystem.getEngine() }
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
		m_viewport.initialise();
		bool result = doInitialiseHiPassProgram();
		Size size = m_renderTarget.getSize();

		if ( result )
		{
			result = doInitialiseCombineProgram();
		}

		uint32_t index = MinTextureIndex;

		for ( auto & surface : m_hiPassSurfaces )
		{
			size.getWidth() >>= 1;
			size.getHeight() >>= 1;
			surface.initialise( m_renderTarget
				, size
				, index++
				, m_linearSampler );
		}

		size = m_renderTarget.getSize();
		index = MinTextureIndex;

		for ( auto & surface : m_blurSurfaces )
		{
			size.getWidth() >>= 1;
			size.getHeight() >>= 1;
			surface.initialise( m_renderTarget
				, size
				, index++
				, m_nearestSampler );
		}

		size = m_renderTarget.getSize();
		m_combineSurface.initialise( m_renderTarget
			, size
			, 0u
			, m_nearestSampler );

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

		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();

		m_combineSurface.cleanup();

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
			auto const & texture = *std::static_pointer_cast< FrameBufferAttachment >( attach )->getTexture();

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
				, *m_combineSurface.m_colourTexture.getTexture() );
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
		m_combineSurface.m_fbo->bind( FrameBufferTarget::eDraw );
		m_combineSurface.m_fbo->clear( BufferComponent::eColour );
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
		p_origin.bind( MinTextureIndex + 4 );
		m_linearSampler->bind( MinTextureIndex + 4 );

		m_geometryBuffers->draw( uint32_t( m_vertices.size() ), 0u );

		texture0.unbind();
		texture1.unbind();
		texture2.unbind();
		texture3.unbind();
		p_origin.unbind( MinTextureIndex + 4 );
		m_linearSampler->unbind( MinTextureIndex + 4 );

		m_combineSurface.m_fbo->unbind();

		m_combineSurface.m_colourTexture.bind();
		m_combineSurface.m_colourTexture.getTexture()->generateMipmaps();
		m_combineSurface.m_colourTexture.unbind();
	}

	SamplerSPtr BloomPostEffect::doCreateSampler( bool p_linear )
	{
		String name = cuT( "BloomSampler_" );
		renderer::Filter mode;

		if ( p_linear )
		{
			name += cuT( "Linear" );
			mode = renderer::Filter::eLinear;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = renderer::Filter::eNearest;
		}

		SamplerSPtr sampler;

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( mode );
			sampler->setMagFilter( mode );
			sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			sampler->setWrapR( renderer::WrapMode::eClampToEdge );
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
		program->createObject( renderer::ShaderStageFlag::eVertex );
		program->createObject( renderer::ShaderStageFlag::eFragment );
		program->createUniform < UniformType::eSampler >( cuT( "c3d_mapDiffuse" )
			, renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex );
		program->setSource( renderer::ShaderStageFlag::eVertex, vertex );
		program->setSource( renderer::ShaderStageFlag::eFragment, hipass );
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
		program->createObject( renderer::ShaderStageFlag::eVertex );
		program->createObject( renderer::ShaderStageFlag::eFragment );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass0
			, renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 0 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass1
			, renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 1 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass2
			, renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 2 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapPass3
			, renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 3 );
		program->createUniform< UniformType::eSampler >( BloomPostEffect::CombineMapScene
			, renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 4 );

		program->setSource( renderer::ShaderStageFlag::eVertex, vertex );
		program->setSource( renderer::ShaderStageFlag::eFragment, combine );
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
