#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlContext.hpp"

#if defined( _WIN32 )
#	include "GlRenderSystem/GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlRenderSystem/GlX11Context.hpp"
#endif

#include "GlRenderSystem/GlRenderSystem.hpp"

#include <Castor3D/RenderWindow.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GlRender;

//*************************************************************************************************

GlBlendState :: GlBlendState( OpenGl & p_gl )
	:	BlendState	(		)
	,	m_gl		( p_gl	)
{
}

GlBlendState :: ~GlBlendState()
{
}

bool GlBlendState :: Apply()
{
	bool l_bReturn = true;
	bool l_bEnabled = false;

	if( m_bEnableAlphaToCoverage )
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_ALPHA_TO_COVERAGE );
		l_bReturn &= m_gl.SampleCoverage( float( m_uiSampleMask / double( 0xFFFFFFFF ) ), false );
		l_bEnabled = true;
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_ALPHA_TO_COVERAGE );
	}

	if( m_bIndependantBlend )
	{
		for( int i = 0; i < 8; i++ )
		{
			if( m_rtStates[i].m_bEnableBlend )
			{
				l_bEnabled = true;
				l_bReturn &= m_gl.BlendFunc( i, m_gl.Get( m_rtStates[i].m_eRgbSrcBlend ), m_gl.Get( m_rtStates[i].m_eRgbDstBlend ), m_gl.Get( m_rtStates[i].m_eAlphaSrcBlend ), m_gl.Get( m_rtStates[i].m_eAlphaDstBlend ) );
				l_bReturn &= m_gl.BlendEquation( i, m_gl.Get( m_rtStates[i].m_eRgbBlendOp ) );
			}
			else
			{
				l_bReturn &= m_gl.BlendFunc( i, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
				l_bReturn &= m_gl.BlendEquation( i, eGL_BLEND_OP_ADD );
			}
		}
	}
	else
	{
		if( m_rtStates[0].m_bEnableBlend )
		{
			l_bEnabled = true;
			l_bReturn &= m_gl.BlendFunc( m_gl.Get( m_rtStates[0].m_eRgbSrcBlend ), m_gl.Get( m_rtStates[0].m_eRgbDstBlend ), m_gl.Get( m_rtStates[0].m_eAlphaSrcBlend ), m_gl.Get( m_rtStates[0].m_eAlphaDstBlend ) );
			l_bReturn &= m_gl.BlendEquation( m_gl.Get( m_rtStates[0].m_eRgbBlendOp ) );
		}
		else
		{
			l_bReturn &= m_gl.BlendFunc( eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
			l_bReturn &= m_gl.BlendEquation( eGL_BLEND_OP_ADD );
		}
	}

	if( l_bEnabled )
	{
		l_bReturn &= m_gl.BlendColor( m_blendFactors );
		l_bReturn &= m_gl.Enable( eGL_TWEAK_BLEND );
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_BLEND );
	}

	m_bChanged = false;
	return l_bReturn;
}

//*************************************************************************************************

GlRasteriserState :: GlRasteriserState( OpenGl & p_gl )
	:	RasteriserState	(		)
	,	m_gl			( p_gl	)
{
}

GlRasteriserState :: ~GlRasteriserState()
{
}

bool GlRasteriserState :: Apply()
{
	bool l_bReturn = true;

	l_bReturn &= m_gl.PolygonMode( eGL_FACE_FRONT_AND_BACK, m_gl.Get( m_eFillMode ) );

	if( m_eCulledFaces != eFACE_NONE )
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_CULL_FACE );
		l_bReturn &= m_gl.CullFace( m_gl.Get( m_eCulledFaces ) );

		if( m_bFrontCCW )
		{
			l_bReturn &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
		}
		else
		{
			l_bReturn &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
		}
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_CULL_FACE );
	}

	if( m_bAntialiasedLines )
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_LINE_SMOOTH );
		l_bReturn &= m_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_NICEST );
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_LINE_SMOOTH );
		l_bReturn &= m_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_DONTCARE );
	}

	if( m_bScissor )
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_SCISSOR_TEST );
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_SCISSOR_TEST );
	}

	if( m_bMultisampled )
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_MULTISAMPLE );
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_MULTISAMPLE );
	}

	if( m_bDepthClipping )
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_DEPTH_CLAMP );
	}
	else
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_DEPTH_CLAMP );
	}

	l_bReturn &= m_gl.PolygonOffset( m_fDepthBias, 4096.0 );

	m_bChanged = false;
	return l_bReturn;
}

//*************************************************************************************************

GlDepthStencilState :: GlDepthStencilState( OpenGl & p_gl )
	:	DepthStencilState	(		)
	,	m_gl				( p_gl	)
{
}

GlDepthStencilState :: ~GlDepthStencilState()
{
}

bool GlDepthStencilState :: Apply()
{
	bool l_bReturn = true;
	
	m_gl.DepthMask( m_gl.Get( m_eDepthMask ) );

	if( m_bDepthTest )
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_DEPTH_TEST );
		l_bReturn &= m_gl.DepthFunc( m_gl.Get( m_eDepthFunc ) );
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_DEPTH_TEST );
	}

	if( m_bStencilTest )
	{
		l_bReturn &= m_gl.Enable( eGL_TWEAK_STENCIL_TEST );
		l_bReturn &= m_gl.StencilMaskSeparate( eGL_FACE_FRONT_AND_BACK, m_uiStencilWriteMask );
		l_bReturn &= m_gl.StencilFuncSeparate( eGL_FACE_FRONT, m_gl.Get( m_stStencilFront.m_eFunc ), 0xFFFFFFFF, m_uiStencilReadMask );
		l_bReturn &= m_gl.StencilFuncSeparate( eGL_FACE_BACK, m_gl.Get( m_stStencilBack.m_eFunc ), 0xFFFFFFFF, m_uiStencilReadMask );
		l_bReturn &= m_gl.StencilOpSeparate( eGL_FACE_FRONT, m_gl.Get( m_stStencilFront.m_eFailOp ), m_gl.Get( m_stStencilFront.m_eDepthFailOp ), m_gl.Get( m_stStencilFront.m_ePassOp ) );
		l_bReturn &= m_gl.StencilOpSeparate( eGL_FACE_BACK, m_gl.Get( m_stStencilFront.m_eFailOp ), m_gl.Get( m_stStencilFront.m_eDepthFailOp ), m_gl.Get( m_stStencilFront.m_ePassOp ) );
	}
	else
	{
		l_bReturn &= m_gl.Disable( eGL_TWEAK_STENCIL_TEST );
	}

	m_bChanged = false;
	return l_bReturn;
}

//*************************************************************************************************

GlContext :: GlContext( OpenGl & p_gl )
	:	m_pGlRenderSystem	( NULL	)
	,	m_gl				( p_gl	)
{
	m_pImplementation = new GlContextImpl( m_gl, this );
}

GlContext :: ~GlContext()
{
	delete m_pImplementation;
}

GlContextImpl * GlContext :: GetImpl()
{
	return m_pImplementation;
}

void GlContext :: UpdateFullScreen( bool p_bVal )
{
	if( Context::m_pWindow->GetVSync() )
	{
		GetImpl()->UpdateVSync( p_bVal );
	}
}

bool GlContext :: DoInitialise()
{
	m_pGlRenderSystem = static_cast< GlRenderSystem * >( m_pRenderSystem );
	m_bInitialised = m_pImplementation->Initialise( m_pWindow );

	if( m_bInitialised )
	{
		SetCurrent();
		GLSL::KeywordsBase * l_pKeywords;
		GLSL::ConstantsBase * l_pConstants;

		switch( m_gl.GetGlslVersion() )
		{
		case 110:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
		case 120:	l_pKeywords = new GLSL::Keywords< 120 >;	break;
		case 130:	l_pKeywords = new GLSL::Keywords< 130 >;	break;
		case 140:	l_pKeywords = new GLSL::Keywords< 140 >;	break;
		case 150:	l_pKeywords = new GLSL::Keywords< 150 >;	break;
		case 330:	l_pKeywords = new GLSL::Keywords< 330 >;	break;
		case 400:	l_pKeywords = new GLSL::Keywords< 400 >;	break;
		case 410:	l_pKeywords = new GLSL::Keywords< 410 >;	break;
		case 420:	l_pKeywords = new GLSL::Keywords< 420 >;	break;
		case 430:	l_pKeywords = new GLSL::Keywords< 430 >;	break;
		default:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
		}

		m_gl.PixelStore( eGL_STORAGE_MODE_UNPACK_ALIGNMENT, 1 );

		if( m_gl.HasUbo() )
		{
			l_pConstants = &GLSL::constantsUbo;
		}
		else
		{
			l_pConstants = &GLSL::constantsStd;
		}

		String l_strVtxShader;
		l_strVtxShader += l_pKeywords->GetVersion();
		l_strVtxShader += l_pKeywords->GetAttribute( 0 )	+	cuT( "	vec4	vertex;\n"										);
		l_strVtxShader += l_pKeywords->GetAttribute( 1 )	+	cuT( "	vec2	texture;\n"										);
		l_strVtxShader += l_pKeywords->GetOut()				+	cuT( "	vec2	vtx_texture;\n"										);
		l_strVtxShader += l_pConstants->Matrices();
		l_strVtxShader +=								cuT( "void main(void)\n"											);
		l_strVtxShader +=								cuT( "{\n"															);
		l_strVtxShader +=								cuT( "	vtx_texture 	= texture.xy;\n"							);
		l_strVtxShader +=								cuT( "	gl_Position 	= c3d_mtxProjectionModelView * vertex;\n"	);
		l_strVtxShader +=								cuT( "}\n"															);

		String l_strPxlShader;
		l_strPxlShader += l_pKeywords->GetVersion();
		l_strPxlShader += l_pKeywords->GetIn()		+	cuT( "	vec2	vtx_texture;\n"											);
		l_strPxlShader +=								cuT( "uniform sampler2D	c3d_mapDiffuse;\n"									);
		l_strPxlShader += l_pKeywords->GetPixelOut();
		l_strPxlShader +=								cuT( "void main()\n"														);
		l_strPxlShader +=								cuT( "{\n"																	);
		l_strPxlShader +=								cuT( "	" ) + l_pKeywords->GetPixelOutputName() + cuT( " = texture2D( c3d_mapDiffuse, vtx_texture );\n"			);
		l_strPxlShader +=								cuT( "}\n"																	);

		delete l_pKeywords;
		GLSL::ConstantsBase::Replace( l_strVtxShader );
		GLSL::ConstantsBase::Replace( l_strPxlShader );

		ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
		l_pProgram->CreateFrameVariable( cuT( "c3d_mapDiffuse" ), eSHADER_TYPE_PIXEL );
		l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL_2, l_strVtxShader );
		l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL_2, l_strPxlShader );
		l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL_3, l_strVtxShader );
		l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL_3, l_strPxlShader );
		l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL_4, l_strVtxShader );
		l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL_4, l_strPxlShader );
		l_pProgram->Initialise();

		m_pGeometryBuffers->GetIndexBuffer()->Create();
		m_pGeometryBuffers->GetVertexBuffer()->Create();
		m_pGeometryBuffers->GetIndexBuffer()->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_pGeometryBuffers->GetVertexBuffer()->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
		m_pGeometryBuffers->Initialise();

#if !defined( NDEBUG )
		l_pProgram->Begin( 0, 1 );
		l_pProgram->End();
#endif

		EndCurrent();
	}

	return m_bInitialised;
}

void GlContext :: DoCleanup()
{
}

void GlContext :: DoSetCurrent()
{
	GetImpl()->SetCurrent();
}

void GlContext :: DoEndCurrent()
{
	GetImpl()->EndCurrent();
}

void GlContext :: DoSwapBuffers()
{
	GetImpl()->SwapBuffers();
}

void GlContext :: DoSetClearColour( Colour const & p_clrClear )
{
	uint8_t l_r, l_g, l_b, l_a;
	m_gl.ClearColor( p_clrClear.red().convert_to( l_r ), p_clrClear.green().convert_to( l_g ), p_clrClear.blue().convert_to( l_b ), p_clrClear.alpha().convert_to( l_a ) );
}

void GlContext :: DoClear( uint32_t p_uiTargets )
{
	m_gl.Clear( m_gl.GetComponents( p_uiTargets ) );
}

void GlContext :: DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget )
{
	if( p_eTarget == eFRAMEBUFFER_TARGET_DRAW )
	{
		if( m_gl.HasFbo() )
		{
			m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, 0 );
		}

		m_gl.DrawBuffer( m_gl.Get( p_eBuffer ) );
	}
	else if( p_eTarget == eFRAMEBUFFER_TARGET_READ )
	{
		if( m_gl.HasFbo() )
		{
			m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, 0 );
		}

		m_gl.ReadBuffer( m_gl.Get( p_eBuffer ) );
	}
}

void GlContext :: DoSetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue )
{
	m_gl.AlphaFunc( m_gl.Get( p_eFunc ), p_byValue / 255.0f );
}

void GlContext :: DoCullFace( eFACE p_eCullFace )
{
	m_gl.CullFace( m_gl.Get( p_eCullFace ) );
}