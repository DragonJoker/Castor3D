#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlOverlayRenderer.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/OpenGl.hpp"

#include <Castor3D/ShaderManager.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

static String PanelVS =
	cuT(	"void main()\n"												)
	cuT(	"{\n"														)
	cuT(	"	vtx_texture = texture;\n"								)
	cuT(	"	gl_Position = c3d_mtxProjectionModelView * vec4( vertex.xyz, 1.0 );\n"	)
	cuT(	"}\n"														);

static String PanelPSDecl =
	cuT(	"<pass_buffer>\n"											)
	cuT(	"out vec4 pxl_v4FragColor;\n"								);

static String PanelPSMain =
	cuT(	"void main()\n"												)
	cuT(	"{\n"														)
	cuT(	"	vec4	l_v4Ambient	= c3d_v4MatAmbient;\n"				)
	cuT(	"	float	l_fAlpha	= c3d_fMatOpacity;\n"				);

static String PanelPSEnd =
	cuT(	"	pxl_v4FragColor = vec4( l_v4Ambient.xyz, l_fAlpha );\n"	)
	cuT(	"}\n"														);

//*************************************************************************************************

GlOverlayRenderer :: GlOverlayRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	OverlayRenderer	( p_pRenderSystem	)
	,	m_gl			( p_gl				)
{
}

GlOverlayRenderer :: ~GlOverlayRenderer()
{
}

ShaderProgramBaseSPtr GlOverlayRenderer :: DoGetProgram( uint32_t p_uiFlags )
{
	GLSL::ConstantsBase * l_pConstants = NULL;
	GLSL::KeywordsBase * l_pKeywords = NULL;

	if( m_gl.HasUbo() )
	{
		l_pConstants = &GLSL::constantsUbo;
	}
	else
	{
		l_pConstants = &GLSL::constantsStd;
	}

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

	String l_strVs;
	l_strVs += l_strVersion;
	l_strVs += l_strAttribute0	+ cuT( "	<vec4> 	vertex;\n"			);
	l_strVs += l_strAttribute4	+ cuT( "	<vec2> 	texture;\n"			);
	l_strVs += l_strOut			+ cuT( "	<vec2> 	vtx_texture;\n"		);
	l_strVs += l_pConstants->Matrices();
	l_strVs += PanelVS;
	GLSL::ConstantsBase::Replace( l_strVs );


	String l_strPs, l_strPsMain;
	l_strPs += l_strVersion;
	l_strPs += l_strIn			+ cuT( "	<vec2> 	vtx_texture;\n"		);
	l_strPs	+= PanelPSDecl;

	l_strPsMain += PanelPSMain;

	if( (p_uiFlags & eTEXTURE_CHANNEL_COLOUR) == eTEXTURE_CHANNEL_COLOUR )
	{
		l_strPs		+= cuT( "uniform sampler2D 	c3d_mapColour;\n" );
		l_strPsMain	+= cuT( "	l_v4Ambient		= texture2D( c3d_mapColour, vtx_texture.xy );\n" );
	}

	if( (p_uiFlags & eTEXTURE_CHANNEL_OPACITY) == eTEXTURE_CHANNEL_OPACITY )
	{
		l_strPs		+= cuT( "uniform sampler2D 	c3d_mapOpacity;\n" );
		l_strPsMain	+= cuT( "	l_fAlpha		= texture2D( c3d_mapColour, vtx_texture.xy ).r;\n" );
	}

	l_strPs += l_strPsMain;
	l_strPs += PanelPSEnd;

	str_utils::replace( l_strPs, cuT( "<pass_buffer>" ), l_pConstants->Pass() );
	GLSL::ConstantsBase::Replace( l_strPs );

	ShaderProgramBaseSPtr l_pProgram = m_pRenderSystem->GetEngine()->GetShaderManager().GetNewProgram();

	for( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
	{
		l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL( i ), l_strVs );
		l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL( i ), l_strPs );
	}

	l_pProgram->Initialise();

	return l_pProgram;
}

void GlOverlayRenderer :: DoInitialise()
{
}

void GlOverlayRenderer :: DoCleanup()
{
}

//*************************************************************************************************
