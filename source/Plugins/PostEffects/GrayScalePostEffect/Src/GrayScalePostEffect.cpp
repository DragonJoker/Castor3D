#include "GrayScalePostEffect.hpp"

#include <Engine.hpp>
#include <SamplerCache.hpp>
#include <ShaderCache.hpp>

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
#include <Render/Pipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
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
		Castor::String GetVertexProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			Vec2 position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetFragmentProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_colour, texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_average, Float( 0.2126f ) * l_colour.SWIZZLE_R + 0.7152f * l_colour.SWIZZLE_G + 0.0722f * l_colour.SWIZZLE_B );
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
	{
		String l_name = cuT( "GrayScale" );

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( l_name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( l_name );
			m_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Nearest );
			m_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Nearest );
			m_sampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::W, WrapMode::ClampToBorder );
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
		bool l_return = false;
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderModel l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		Size l_size = m_renderTarget.GetSize();

		auto l_vertex = GetVertexProgram( GetRenderSystem() );
		auto l_fragment = GetFragmentProgram( GetRenderSystem() );

		if ( !l_vertex.empty() && !l_fragment.empty() )
		{
			ShaderProgramSPtr l_program = l_cache.GetNewProgram();
			m_mapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::Pixel );
			l_cache.CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );
			l_program->SetSource( ShaderType::Vertex, l_model, l_vertex );
			l_program->SetSource( ShaderType::Pixel, l_model, l_fragment );
			l_program->Initialise();

			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::Zero );
			RasteriserState l_rsstate;
			l_rsstate.SetCulledFaces( Culling::Back );
			m_pipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsstate ), std::move( l_rsstate ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
		}

		return m_surface.Initialise( m_renderTarget, l_size, 0, m_sampler );
	}

	void GrayScalePostEffect::Cleanup()
	{
		m_mapDiffuse.reset();
		m_surface.Cleanup();
	}

	bool GrayScalePostEffect::Apply( FrameBuffer & p_framebuffer )
	{
		auto l_attach = p_framebuffer.GetAttachment( AttachmentPoint::Colour, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == AttachmentType::Texture )
		{
			bool l_return = m_surface.m_fbo->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw );
			auto l_texture = std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture();

			if ( l_return )
			{
				m_surface.m_fbo->Clear();
				m_mapDiffuse->SetValue( 0 );
				GetRenderSystem()->GetCurrentContext()->RenderTexture( m_surface.m_size, *l_texture, *m_pipeline );
				m_surface.m_fbo->Unbind();
			}

			if ( p_framebuffer.Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw ) )
			{
				GetRenderSystem()->GetCurrentContext()->RenderTexture( l_texture->GetDimensions(), *m_surface.m_colourTexture.GetTexture() );
				p_framebuffer.Unbind();
			}
		}

		return true;
	}

	bool GrayScalePostEffect::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
