#include "GlOverlayRenderer.hpp"
#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

#include <ShaderManager.hpp>
#include <ShaderProgram.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

static String OverlayVS =
	cuT(	"void main()\n"	)
	cuT(	"{\n"	)
	cuT(	"	vtx_texture = texture;\n" )
	cuT(	"	gl_Position = c3d_mtxProjection * vec4( vertex.xyz, 1.0 );\n" )
	cuT(	"}\n"	);

static String PanelPSDecl =
	cuT(	"<pass_buffer>\n" )
	cuT(	"out vec4 pxl_v4FragColor;\n" );

static String PanelPSMain =
	cuT(	"void main()\n" )
	cuT(	"{\n" )
	cuT(	"	vec4	l_v4Ambient	= c3d_v4MatAmbient;\n" )
	cuT(	"	float	l_fAlpha	= c3d_fMatOpacity;\n" );

static String PanelPSEnd =
	cuT(	"	pxl_v4FragColor = vec4( l_v4Ambient.xyz, l_fAlpha );\n" )
	cuT(	"}\n" );

//*************************************************************************************************

GlOverlayRenderer::GlOverlayRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	OverlayRenderer( p_pRenderSystem )
	,	m_gl( p_gl	)
{
}

GlOverlayRenderer::~GlOverlayRenderer()
{
}

ShaderProgramBaseSPtr GlOverlayRenderer::DoGetProgram( uint32_t p_uiFlags )
{
	GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
	GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
	std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

	String l_strVersion		= l_pKeywords->GetVersion();
	String l_strAttribute0	= l_pKeywords->GetAttribute( 0 );
	String l_strAttribute1	= l_pKeywords->GetAttribute( 1 );
	String l_strAttribute2	= l_pKeywords->GetAttribute( 2 );
	String l_strAttribute3	= l_pKeywords->GetAttribute( 3 );
	String l_strAttribute4	= l_pKeywords->GetAttribute( 4 );
	String l_strAttribute5	= l_pKeywords->GetAttribute( 5 );
	String l_strAttribute6	= l_pKeywords->GetAttribute( 6 );
	String l_strAttribute7	= l_pKeywords->GetAttribute( 7 );
	String l_strIn			= l_pKeywords->GetIn();
	String l_strOut			= l_pKeywords->GetOut();

	// Vertex shader
	String l_strVs;
	l_strVs += l_strVersion;
	l_strVs += l_strAttribute0	+ cuT( "	ivec3 	vertex;\n"	);
	l_strVs += l_strAttribute1	+ cuT( "	<vec2> 	texture;\n"	);
	l_strVs += l_strOut			+ cuT( "	<vec2> 	vtx_texture;\n"	);
	l_strVs += l_pConstants->Matrices();
	l_strVs += OverlayVS;
	str_utils::replace( l_strVs, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	GLSL::ConstantsBase::Replace( l_strVs );

	// Pixel shader
	String l_strPs, l_strPsMain;
	l_strPs += l_strVersion;
	l_strPs += l_strIn			+ cuT( "	<vec2> 	vtx_texture;\n"	);
	l_strPs	+= PanelPSDecl;
	l_strPsMain += PanelPSMain;

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
	{
		l_strPs		+= cuT( "uniform sampler2D 	c3d_mapText;\n" );
		l_strPsMain	+= cuT( "	l_fAlpha		*= <texture2D>( c3d_mapText, vtx_texture.xy ).r;\n" );
	}

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
	{
		l_strPs		+= cuT( "uniform sampler2D 	c3d_mapColour;\n" );
		l_strPsMain	+= cuT( "	l_v4Ambient		= <texture2D>( c3d_mapColour, vtx_texture.xy );\n" );
	}

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
	{
		l_strPs		+= cuT( "uniform sampler2D 	c3d_mapOpacity;\n" );
		l_strPsMain	+= cuT( "	l_fAlpha		*= <texture2D>( c3d_mapOpacity, vtx_texture.xy ).r;\n" );
	}

	l_strPs += l_strPsMain;
	l_strPs += PanelPSEnd;
	str_utils::replace( l_strPs, cuT( "<pass_buffer>" ), l_pConstants->Pass() );
	str_utils::replace( l_strPs, cuT( "<texture2D>" ), l_pKeywords->GetTexture2D() );
	str_utils::replace( l_strPs, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	GLSL::ConstantsBase::Replace( l_strPs );

	// Shader program
	ShaderProgramBaseSPtr l_pProgram = m_pRenderSystem->GetEngine()->GetShaderManager().GetNewProgram();

	for ( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
	{
		l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL( i ), l_strVs );
		l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL( i ), l_strPs );
	}

	return l_pProgram;
}

void GlOverlayRenderer::DoInitialise()
{
}

void GlOverlayRenderer::DoCleanup()
{
}

//*************************************************************************************************
