#include "GrayScalePostEffect.hpp"

#include <BlendStateManager.hpp>
#include <Engine.hpp>
#include <SamplerManager.hpp>
#include <ShaderManager.hpp>

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
				gl_Position = c3d_mtxProjection * vec4( position.SWIZZLE_XY, 0.0, 1.0 );
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
				plx_v4FragColor = vec4( texture2D( c3d_mapDiffuse, vec2( vtx_texture.SWIZZLE_X, vtx_texture.SWIZZLE_Y ) ).SWIZZLE_XYZ, 1.0 );
				LOCALE_ASSIGN( l_writer, Float, l_average, Float( 0.2126f ) * plx_v4FragColor.SWIZZLE_R + 0.7152f * plx_v4FragColor.SWIZZLE_G + 0.0722f * plx_v4FragColor.SWIZZLE_B );
				plx_v4FragColor = vec4( l_average, l_average, l_average, 1.0 );
			} );
			return l_writer.Finalise();
		}
	}

	//*********************************************************************************************

	GrayScalePostEffect::PostEffectSurface::PostEffectSurface()
	{
	}

	bool GrayScalePostEffect::PostEffectSurface::Initialise( RenderTarget & p_renderTarget, Size const & p_size, uint32_t p_index, SamplerSPtr p_sampler )
	{
		bool l_return = false;
		m_size = p_size;
		m_colourTexture = std::make_shared< TextureUnit >( *p_renderTarget.GetEngine() );
		m_colourTexture->SetIndex( p_index );

		m_fbo = p_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		auto l_colourTexture = p_renderTarget.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );

		m_colourTexture->SetSampler( p_sampler );
		//l_colourTexture->GetImage().SetSource( p_renderTarget );
		l_colourTexture->GetImage().SetSource( p_size, ePIXEL_FORMAT_A8R8G8B8 );
		m_colourAttach = m_fbo->CreateAttachment( l_colourTexture );

		m_fbo->Create();
		m_colourTexture->SetTexture( l_colourTexture );
		m_colourTexture->Initialise();
		m_fbo->Initialise( p_size );
		m_fbo->SetClearColour( Colour::from_predef( Colour::ePREDEFINED_FULLALPHA_BLACK ) );

		if ( m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			m_fbo->Attach( eATTACHMENT_POINT_COLOUR, 0, m_colourAttach, l_colourTexture->GetType() );
			l_return = m_fbo->IsComplete();
			m_fbo->Unbind();
		}

		return l_return;
	}

	void GrayScalePostEffect::PostEffectSurface::Cleanup()
	{
		m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_fbo->DetachAll();
		m_fbo->Unbind();
		m_fbo->Cleanup();

		m_colourTexture->Cleanup();
		m_fbo->Destroy();

		m_fbo.reset();
		m_colourAttach.reset();
		m_colourTexture.reset();
	}

	//*********************************************************************************************

	String GrayScalePostEffect::Type = cuT( "grayscale" );
	String GrayScalePostEffect::Name = cuT( "GrayScale PostEffect" );

	GrayScalePostEffect::GrayScalePostEffect( RenderSystem & p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_param )
		: PostEffect( p_renderSystem, p_renderTarget, GrayScalePostEffect::Type, p_param )
	{
		String l_name = cuT( "GrayScaleSampler" );

		if ( !m_renderTarget.GetEngine()->GetSamplerManager().Has( l_name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerManager().Create( l_name );
			m_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Nearest );
			m_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Nearest );
			m_sampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::W, WrapMode::ClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerManager().Find( l_name );
		}
	}

	GrayScalePostEffect::~GrayScalePostEffect()
	{
	}

	bool GrayScalePostEffect::Initialise()
	{
		bool l_return = false;
		ShaderManager & l_manager = GetRenderSystem()->GetEngine()->GetShaderManager();
		eSHADER_MODEL l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		Size l_size = m_renderTarget.GetSize();

		auto l_vertex = GetVertexProgram( GetRenderSystem() );
		auto l_fragment = GetFragmentProgram( GetRenderSystem() );

		if ( !l_vertex.empty() && !l_fragment.empty() )
		{
			ShaderProgramSPtr l_program = l_manager.GetNewProgram();
			m_mapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, eSHADER_TYPE_PIXEL );
			l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
			l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vertex );
			l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_fragment );
			l_program->Initialise();
			m_program = l_program;
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
		auto l_attach = p_framebuffer.GetAttachment( eATTACHMENT_POINT_COLOUR, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
		{
			bool l_return = m_surface.m_fbo->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
			auto l_texture = std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture();

			if ( l_return )
			{
				m_surface.m_fbo->Clear();
				m_mapDiffuse->SetValue( 0 );
				GetRenderSystem()->GetCurrentContext()->RenderTexture( m_surface.m_size, *l_texture, m_program.lock() );
				m_surface.m_fbo->Unbind();
			}

			if ( p_framebuffer.Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
			{
				GetRenderSystem()->GetCurrentContext()->RenderTexture( l_texture->GetImage().GetDimensions(), *m_surface.m_colourTexture->GetTexture() );
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
