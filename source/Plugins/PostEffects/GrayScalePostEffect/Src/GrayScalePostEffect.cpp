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
		GLSL::Shader GetVertexProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			Vec2 position = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = l_writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return l_writer.Finalise();
		}

		GLSL::Shader GetFragmentProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto l_colour = l_writer.DeclLocale( cuT( "l_colour" ), texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz() );
				auto l_average = l_writer.DeclLocale( cuT( "l_average" ), Float( 0.2126f ) * l_colour.r() + 0.7152f * l_colour.g() + 0.0722f * l_colour.b() );
				plx_v4FragColor = vec4( l_average, l_average, l_average, 1.0 );
			} );
			return l_writer.Finalise();
		}
	}

	//*********************************************************************************************

	String GrayScalePostEffect::Type = cuT( "grayscale" );
	String GrayScalePostEffect::Name = cuT( "GrayScale PostEffect" );

	GrayScalePostEffect::GrayScalePostEffect( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_param )
		: PostEffect{ GrayScalePostEffect::Type, p_renderTarget, p_renderSystem, p_param }
		, m_surface{ *p_renderSystem.GetEngine() }
		, m_matrixUbo{ *p_renderSystem.GetEngine() }
	{
		String l_name = cuT( "GrayScale" );

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( l_name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( l_name );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
			m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( l_name );
		}
	}

	GrayScalePostEffect::~GrayScalePostEffect()
	{
	}

	PostEffectSPtr GrayScalePostEffect::Create( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_param )
	{
		return std::make_shared< GrayScalePostEffect >( p_renderTarget, p_renderSystem, p_param );
	}

	bool GrayScalePostEffect::Initialise()
	{
		bool l_result = false;
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		Size l_size = m_renderTarget.GetSize();

		auto l_vertex = GetVertexProgram( GetRenderSystem() );
		auto l_fragment = GetFragmentProgram( GetRenderSystem() );
		ShaderProgramSPtr l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		m_mapDiffuse = l_program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_vertex );
		l_program->SetSource( ShaderType::ePixel, l_fragment );
		l_program->Initialise();

		DepthStencilState l_dsstate;
		l_dsstate.SetDepthTest( false );
		l_dsstate.SetDepthMask( WritingMask::eZero );
		RasteriserState l_rsstate;
		l_rsstate.SetCulledFaces( Culling::eBack );
		m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate ), std::move( l_rsstate ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		return m_surface.Initialise( m_renderTarget, l_size, 0, m_sampler );
	}

	void GrayScalePostEffect::Cleanup()
	{
		m_mapDiffuse.reset();
		m_surface.Cleanup();
	}

	bool GrayScalePostEffect::Apply( FrameBuffer & p_framebuffer )
	{
		auto l_attach = p_framebuffer.GetAttachment( AttachmentPoint::eColour, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == AttachmentType::eTexture )
		{
			m_surface.m_fbo->Bind( FrameBufferTarget::eDraw );
			auto l_texture = std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture();
			m_surface.m_fbo->Clear( BufferComponent::eColour );
			m_mapDiffuse->SetValue( 0 );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( 
				m_surface.m_size
				, *l_texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->Unbind();

			p_framebuffer.Bind( FrameBufferTarget::eDraw );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( l_texture->GetDimensions(), *m_surface.m_colourTexture.GetTexture() );
			p_framebuffer.Unbind();
		}

		return true;
	}

	bool GrayScalePostEffect::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
