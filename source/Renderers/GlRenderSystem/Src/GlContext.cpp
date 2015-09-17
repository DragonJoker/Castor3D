#include "GlContext.hpp"

#if defined( _WIN32 )
#	include "GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlX11Context.hpp"
#endif

#include "GlRenderSystem.hpp"
#include "GlShaderSource.hpp"

#include <RenderWindow.hpp>
#include <ShaderProgram.hpp>
#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <MatrixBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlContext::GlContext( OpenGl & p_gl )
		:	m_pGlRenderSystem( NULL )
		,	m_gl( p_gl )
	{
		m_pImplementation = new GlContextImpl( m_gl, this );
	}

	GlContext::~GlContext()
	{
		delete m_pImplementation;
	}

	GlContextImpl * GlContext::GetImpl()
	{
		return m_pImplementation;
	}

	void GlContext::UpdateFullScreen( bool p_bVal )
	{
		if ( Context::m_pWindow->GetVSync() )
		{
			GetImpl()->UpdateVSync( p_bVal );
		}
	}

	bool GlContext::DoInitialise()
	{
		using namespace GLSL;
		m_pGlRenderSystem = static_cast< GlRenderSystem * >( m_renderSystem );
		m_bInitialised = m_pImplementation->Initialise( m_pWindow );

		if ( m_bInitialised )
		{
			String l_strVtxShader;
			{
				// Vertex shader
				GlslWriter l_writer( m_gl, eSHADER_TYPE_VERTEX );
				l_writer << Version() << Endl();

				UBO_MATRIX( l_writer );

				// Shader inputs
				ATTRIBUTE( l_writer, Vec4, vertex );
				ATTRIBUTE( l_writer, Vec2, texture );

				// Shader outputs
				OUT( l_writer, Vec2, vtx_texture );

				l_writer.Implement_Function< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texture;
					BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.x(), vertex.y(), vertex.z(), 1.0 );
				} );
				l_strVtxShader = l_writer.Finalise();
			}

			String l_strPxlShader;
			{
				GlslWriter l_writer( m_gl, eSHADER_TYPE_PIXEL );
				l_writer << Version() << Endl();

				// Shader inputs
				UNIFORM( l_writer, Sampler2D, c3d_mapDiffuse );
				IN( l_writer, Vec2, vtx_texture );

				// Shader outputs
				LAYOUT( l_writer, Vec4, plx_v4FragColor );

				l_writer.Implement_Function< void >( cuT( "main" ), [&]()
				{
					plx_v4FragColor = texture2D( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) );
				} );
				l_strPxlShader = l_writer.Finalise();
			}

			ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
			l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_2, l_strVtxShader );
			l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_2, l_strPxlShader );
			l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_3, l_strVtxShader );
			l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_3, l_strPxlShader );
			l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_4, l_strVtxShader );
			l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_4, l_strPxlShader );
		}

		return m_bInitialised;
	}

	void GlContext::DoCleanup()
	{
		m_pImplementation->Cleanup();
	}

	void GlContext::DoSetCurrent()
	{
		GetImpl()->SetCurrent();
	}

	void GlContext::DoEndCurrent()
	{
		GetImpl()->EndCurrent();
	}

	void GlContext::DoSwapBuffers()
	{
		GetImpl()->SwapBuffers();
	}

	void GlContext::DoSetClearColour( Colour const & p_clrClear )
	{
		uint8_t l_r, l_g, l_b, l_a;
		m_gl.ClearColor( p_clrClear.red().convert_to( l_r ), p_clrClear.green().convert_to( l_g ), p_clrClear.blue().convert_to( l_b ), p_clrClear.alpha().convert_to( l_a ) );
	}

	void GlContext::DoClear( uint32_t p_uiTargets )
	{
		m_gl.Clear( m_gl.GetComponents( p_uiTargets ) );
	}

	void GlContext::DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget )
	{
		if ( p_eTarget == eFRAMEBUFFER_TARGET_DRAW )
		{
			if ( m_gl.HasFbo() )
			{
				m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, 0 );
			}

			m_gl.DrawBuffer( m_gl.Get( p_eBuffer ) );
		}
		else if ( p_eTarget == eFRAMEBUFFER_TARGET_READ )
		{
			if ( m_gl.HasFbo() )
			{
				m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, 0 );
			}

			m_gl.ReadBuffer( m_gl.Get( p_eBuffer ) );
		}
	}

	void GlContext::DoSetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue )
	{
		m_gl.AlphaFunc( m_gl.Get( p_eFunc ), p_byValue / 255.0f );
	}

	void GlContext::DoCullFace( eFACE p_eCullFace )
	{
		m_gl.CullFace( m_gl.Get( p_eCullFace ) );
	}
}
