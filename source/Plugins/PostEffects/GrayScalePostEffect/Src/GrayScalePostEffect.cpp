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

using namespace Castor;
using namespace Castor3D;

namespace GrayScale
{
	namespace
	{
		GLSL::Shader GetVertexProgram( RenderSystem * renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = renderSystem->CreateGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			Vec2 position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.Finalise();
		}

		GLSL::Shader GetFragmentProgram( RenderSystem * renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto colour = writer.DeclLocale( cuT( "colour" ), texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz() );
				auto average = writer.DeclLocale( cuT( "average" ), Float( 0.2126f ) * colour.r() + 0.7152f * colour.g() + 0.0722f * colour.b() );
				plx_v4FragColor = vec4( average, average, average, 1.0 );
			} );
			return writer.Finalise();
		}
	}

	//*********************************************************************************************

	String GrayScalePostEffect::Type = cuT( "grayscale" );
	String GrayScalePostEffect::Name = cuT( "GrayScale PostEffect" );

	GrayScalePostEffect::GrayScalePostEffect( RenderTarget & p_renderTarget, RenderSystem & renderSystem, Parameters const & p_param )
		: PostEffect{ GrayScalePostEffect::Type, p_renderTarget, renderSystem, p_param }
		, m_surface{ *renderSystem.GetEngine() }
		, m_matrixUbo{ *renderSystem.GetEngine() }
	{
		String name = cuT( "GrayScale" );

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( name );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
			m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( name );
		}
	}

	GrayScalePostEffect::~GrayScalePostEffect()
	{
	}

	PostEffectSPtr GrayScalePostEffect::Create( RenderTarget & p_renderTarget, RenderSystem & renderSystem, Parameters const & p_param )
	{
		return std::make_shared< GrayScalePostEffect >( p_renderTarget, renderSystem, p_param );
	}

	bool GrayScalePostEffect::Initialise()
	{
		bool result = false;
		auto & cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		Size size = m_renderTarget.GetSize();

		auto vertex = GetVertexProgram( GetRenderSystem() );
		auto fragment = GetFragmentProgram( GetRenderSystem() );
		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		m_mapDiffuse = program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		program->SetSource( ShaderType::eVertex, vertex );
		program->SetSource( ShaderType::ePixel, fragment );
		program->Initialise();

		DepthStencilState dsstate;
		dsstate.SetDepthTest( false );
		dsstate.SetDepthMask( WritingMask::eZero );
		RasteriserState rsstate;
		rsstate.SetCulledFaces( Culling::eBack );
		m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( dsstate ), std::move( rsstate ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		return m_surface.Initialise( m_renderTarget, size, 0, m_sampler );
	}

	void GrayScalePostEffect::Cleanup()
	{
		m_mapDiffuse.reset();
		m_surface.Cleanup();
	}

	bool GrayScalePostEffect::Apply( FrameBuffer & p_framebuffer )
	{
		auto attach = p_framebuffer.GetAttachment( AttachmentPoint::eColour, 0 );

		if ( attach && attach->GetAttachmentType() == AttachmentType::eTexture )
		{
			m_surface.m_fbo->Bind( FrameBufferTarget::eDraw );
			auto texture = std::static_pointer_cast< TextureAttachment >( attach )->GetTexture();
			m_surface.m_fbo->Clear( BufferComponent::eColour );
			m_mapDiffuse->SetValue( 0 );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( 
				m_surface.m_size
				, *texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->Unbind();

			p_framebuffer.Bind( FrameBufferTarget::eDraw );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( texture->GetDimensions(), *m_surface.m_colourTexture.GetTexture() );
			p_framebuffer.Unbind();
		}

		return true;
	}

	bool GrayScalePostEffect::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
