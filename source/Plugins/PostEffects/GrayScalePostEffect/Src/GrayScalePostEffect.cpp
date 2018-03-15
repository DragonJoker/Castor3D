#include "GrayScalePostEffect.hpp"

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
#include <State/BlendState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;
using namespace castor3d;

namespace GrayScale
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

		glsl::Shader getFragmentProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), MinBufferIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" ), texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz() );
				auto average = writer.declLocale( cuT( "average" ), Float( 0.2126f ) * colour.r() + 0.7152f * colour.g() + 0.0722f * colour.b() );
				pxl_fragColor = vec4( average, average, average, 1.0 );
			} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	String GrayScalePostEffect::Type = cuT( "grayscale" );
	String GrayScalePostEffect::Name = cuT( "GrayScale PostEffect" );

	GrayScalePostEffect::GrayScalePostEffect( RenderTarget & p_renderTarget, RenderSystem & renderSystem, Parameters const & p_param )
		: PostEffect{ GrayScalePostEffect::Type, p_renderTarget, renderSystem, p_param }
		, m_surface{ *renderSystem.getEngine() }
		, m_matrixUbo{ *renderSystem.getEngine() }
	{
		String name = cuT( "GrayScale" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( renderer::Filter::eNearest );
			m_sampler->setMagFilter( renderer::Filter::eNearest );
			m_sampler->setWrapS( renderer::WrapMode::eClampToBorder );
			m_sampler->setWrapT( renderer::WrapMode::eClampToBorder );
			m_sampler->setWrapR( renderer::WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}
	}

	GrayScalePostEffect::~GrayScalePostEffect()
	{
	}

	PostEffectSPtr GrayScalePostEffect::create( RenderTarget & p_renderTarget, RenderSystem & renderSystem, Parameters const & p_param )
	{
		return std::make_shared< GrayScalePostEffect >( p_renderTarget, renderSystem, p_param );
	}

	bool GrayScalePostEffect::initialise()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		Size size = m_renderTarget.getSize();

		auto vertex = getVertexProgram( getRenderSystem() );
		auto fragment = getFragmentProgram( getRenderSystem() );
		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( renderer::ShaderStageFlag::eVertex );
		program->createObject( renderer::ShaderStageFlag::eFragment );
		program->createUniform< UniformType::eSampler >( cuT( "c3d_mapDiffuse" ), renderer::ShaderStageFlag::eFragment )->setValue( MinBufferIndex );
		program->setSource( renderer::ShaderStageFlag::eVertex, vertex );
		program->setSource( renderer::ShaderStageFlag::eFragment, fragment );
		program->initialise();

		DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( WritingMask::eZero );
		RasteriserState rsstate;
		rsstate.setCulledFaces( Culling::eBack );
		m_pipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate ), std::move( rsstate ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		return m_surface.initialise( m_renderTarget
			, size
			, MinBufferIndex
			, m_sampler );
	}

	void GrayScalePostEffect::cleanup()
	{
		m_surface.cleanup();
	}

	bool GrayScalePostEffect::apply( FrameBuffer & p_framebuffer )
	{
		auto attach = p_framebuffer.getAttachment( AttachmentPoint::eColour, 0 );

		if ( attach && attach->getAttachmentType() == AttachmentType::eTexture )
		{
			m_surface.m_fbo->bind( FrameBufferTarget::eDraw );
			auto texture = std::static_pointer_cast< FrameBufferAttachment >( attach )->getTexture();
			m_surface.m_fbo->clear( BufferComponent::eColour );
			getRenderSystem()->getCurrentContext()->renderTexture( 
				m_surface.m_size
				, *texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->unbind();

			p_framebuffer.bind( FrameBufferTarget::eDraw );
			getRenderSystem()->getCurrentContext()->renderTexture( texture->getDimensions(), *m_surface.m_colourTexture.getTexture() );
			p_framebuffer.unbind();
		}

		return true;
	}

	bool GrayScalePostEffect::doWriteInto( TextFile & p_file )
	{
		return true;
	}
}
