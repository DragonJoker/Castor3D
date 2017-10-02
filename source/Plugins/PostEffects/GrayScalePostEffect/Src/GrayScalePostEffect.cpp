#include "GrayScalePostEffect.hpp"

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

			UBO_MATRIX( writer );

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( ShaderProgram::Position );

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
			auto c3d_mapDiffuse = writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse );
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
			m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
			m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
			m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
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
		bool result = false;
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		Size size = m_renderTarget.getSize();

		auto vertex = getVertexProgram( getRenderSystem() );
		auto fragment = getFragmentProgram( getRenderSystem() );
		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->setValue( MinTextureIndex );
		program->setSource( ShaderType::eVertex, vertex );
		program->setSource( ShaderType::ePixel, fragment );
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
			, MinTextureIndex
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
			auto texture = std::static_pointer_cast< TextureAttachment >( attach )->getTexture();
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
