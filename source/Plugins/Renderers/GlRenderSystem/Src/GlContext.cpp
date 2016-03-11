#include "GlContext.hpp"

#if defined( _WIN32 )
#	include "GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlX11Context.hpp"
#endif

#include "GlRenderSystem.hpp"

#include <GlslSource.hpp>

#include <RenderWindow.hpp>
#include <ShaderProgram.hpp>
#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlContext::GlContext( GlRenderSystem & p_renderSystem, OpenGl & p_gl )
		: Context( p_renderSystem, false )
		, m_glRenderSystem( &p_renderSystem )
		, Holder( p_gl )
		, m_timerQuery { GlQuery( p_gl, eGL_QUERY_TIME_ELAPSED ), GlQuery( p_gl, eGL_QUERY_TIME_ELAPSED ) }
	{
		m_implementation = new GlContextImpl( GetOpenGl(), this );
	}

	GlContext::~GlContext()
	{
		delete m_implementation;
	}

	GlContextImpl * GlContext::GetImpl()
	{
		return m_implementation;
	}

	void GlContext::UpdateFullScreen( bool p_value )
	{
	}

	bool GlContext::DoInitialise()
	{
		using namespace GLSL;
		m_initialised = m_implementation->Initialise( m_window );

		if ( m_initialised )
		{
			String l_strVtxShader;
			{
				// Vertex shader
				auto l_writer = GetRenderSystem()->CreateGlslWriter();

				UBO_MATRIX( l_writer );

				// Shader inputs
				auto position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );
				auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );

				// Shader outputs
				auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
				auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position.X, position.Y, 0.0, 1.0 );
				} );
				l_strVtxShader = l_writer.Finalise();
			}

			String l_strPxlShader;
			{
				auto l_writer = GetRenderSystem()->CreateGlslWriter();

				// Shader inputs
				auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
				auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

				// Shader outputs
				auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					plx_v4FragColor = vec4( texture2D( c3d_mapDiffuse, vec2( vtx_texture.X, vtx_texture.Y ) ).XYZ, 1.0 );
				} );
				l_strPxlShader = l_writer.Finalise();
			}

			eSHADER_MODEL l_model = m_glRenderSystem->GetMaxShaderModel();
			ShaderProgramSPtr l_program = m_renderTextureProgram.lock();
			l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_strVtxShader );
			l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_strPxlShader );

			m_implementation->SetCurrent();
			m_timerQuery[0].Create();
			m_timerQuery[1].Create();
			m_timerQuery[1 - m_queryIndex].Begin();
			m_timerQuery[1 - m_queryIndex].End();
			m_implementation->EndCurrent();
		}

		return m_initialised;
	}

	void GlContext::DoCleanup()
	{
		m_implementation->SetCurrent();
		m_timerQuery[0].Destroy();
		m_timerQuery[1].Destroy();
		m_implementation->EndCurrent();
		m_implementation->Cleanup();
	}

	void GlContext::DoSetCurrent()
	{
		GetImpl()->SetCurrent();
		m_timerQuery[m_queryIndex].Begin();
	}

	void GlContext::DoEndCurrent()
	{
		m_timerQuery[m_queryIndex].End();
		uint64_t l_time = 0;
		m_queryIndex = 1 - m_queryIndex;
		m_timerQuery[m_queryIndex].GetInfos( eGL_QUERY_INFO_RESULT, l_time );
		GetRenderSystem()->IncGpuTime( std::chrono::nanoseconds( l_time ) );
		GetImpl()->EndCurrent();
	}

	void GlContext::DoSwapBuffers()
	{
		GetImpl()->SwapBuffers();
	}

	void GlContext::DoSetAlphaFunc( eALPHA_FUNC p_func, uint8_t p_value )
	{
		GetOpenGl().AlphaFunc( GetOpenGl().Get( p_func ), p_value / 255.0f );
	}

	void GlContext::DoCullFace( eFACE p_eCullFace )
	{
		if ( p_eCullFace == eFACE_NONE )
		{
			GetOpenGl().Disable( eGL_TWEAK_CULL_FACE );
		}
		else
		{
			GetOpenGl().Enable( eGL_TWEAK_CULL_FACE );
			GetOpenGl().CullFace( GetOpenGl().Get( p_eCullFace ) );
		}
	}
}
