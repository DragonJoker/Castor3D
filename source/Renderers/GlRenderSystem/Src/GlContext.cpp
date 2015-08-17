#include "GlContext.hpp"

#if defined( _WIN32 )
#	include "GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlX11Context.hpp"
#endif

#include "GlRenderSystem.hpp"

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
		m_pGlRenderSystem = static_cast< GlRenderSystem * >( m_pRenderSystem );
		m_bInitialised = m_pImplementation->Initialise( m_pWindow );

		if ( m_bInitialised )
		{
			SetCurrent();
			GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
			GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
			std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

			StringStream l_streamVtxShader;
			l_streamVtxShader << l_pKeywords->GetVersion();
			l_streamVtxShader << l_pConstants->Matrices();
			l_streamVtxShader << l_pKeywords->GetAttribute( 0 ) << cuT( "	<vec4> vertex;\n" );
			l_streamVtxShader << l_pKeywords->GetAttribute( 1 ) << cuT( "	<vec2> texture;\n" );
			l_streamVtxShader << l_pKeywords->GetOut() << cuT( "	<vec2> vtx_texture;\n" );
			l_streamVtxShader << cuT( "void main(void)\n" );
			l_streamVtxShader << cuT( "{\n" );
			l_streamVtxShader << cuT( "	vtx_texture = texture.xy;\n" );
			l_streamVtxShader << cuT( "	gl_Position = c3d_mtxProjection * vec4( vertex.x, vertex.y, vertex.z, 1.0 );\n" );
			l_streamVtxShader << cuT( "}\n" );
			String l_strVtxShader = l_streamVtxShader.str();
			StringStream l_streamPxlShader;
			l_streamPxlShader << l_pKeywords->GetVersion();
			l_streamPxlShader << l_pKeywords->GetIn() << cuT( "	<vec2> vtx_texture;\n" );
			l_streamPxlShader << cuT( "uniform sampler2D c3d_mapDiffuse;\n" );
			l_streamPxlShader << l_pKeywords->GetPixelOut();
			l_streamPxlShader << cuT( "void main()\n" );
			l_streamPxlShader << cuT( "{\n" );
			l_streamPxlShader << cuT( "	" ) << l_pKeywords->GetPixelOutputName() << cuT( " = <texture2D>( c3d_mapDiffuse, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
			l_streamPxlShader << cuT( "}\n" );
			String l_strPxlShader = l_streamPxlShader.str();
			GLSL::ConstantsBase::Replace( l_strVtxShader );
			GLSL::ConstantsBase::Replace( l_strPxlShader );
			str_utils::replace( l_strPxlShader, cuT( "<texture2D>" ), l_pKeywords->GetTexture2D() );
			str_utils::replace( l_strVtxShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
			str_utils::replace( l_strPxlShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
			ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
			l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_2, l_strVtxShader );
			l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_2, l_strPxlShader );
			l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_3, l_strVtxShader );
			l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_3, l_strPxlShader );
			l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_4, l_strVtxShader );
			l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_4, l_strPxlShader );
			l_pProgram->Initialise();
			m_pGeometryBuffers->GetVertexBuffer().Create();
			m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
			m_pGeometryBuffers->Initialise();
#if !defined( NDEBUG )
			l_pProgram->Bind( 0, 1 );
			l_pProgram->Unbind();
#endif
			EndCurrent();
		}

		return m_bInitialised;
	}

	void GlContext::DoCleanup()
	{
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
