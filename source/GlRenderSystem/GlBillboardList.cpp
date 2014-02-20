#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlBillboardList.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlShaderObject.hpp"

using namespace Castor;
using namespace Castor3D;
using namespace GlRender;

//*************************************************************************************************

namespace GLSLBillboard
{
	class VariablesBase
	{
	public:
		virtual String User()=0;
	};

	class VariablesStd : public VariablesBase
	{
	public:
		virtual String User()
		{
			return
				cuT( "uniform ivec2 c3d_v2iDimensions;" );
		}
	};

	class VariablesUbo : public VariablesBase
	{
	public:
		virtual String User()
		{
			return
				cuT( "layout( std140 ) uniform User\n"			)
				cuT( "{\n"										)
				cuT( "	ivec2 c3d_v2iDimensions;\n"				)
				cuT( "};\n"										);
		}
	};

	VariablesStd	variablesStd;
	VariablesUbo	variablesUbo;
}

//*************************************************************************************************

static String BillboardVS =
	cuT( "void main()\n"														)
	cuT( "{\n"																	)
	cuT( "	gl_Position = vertex;\n"											)
	cuT( "}\n"																	);

static String BillboardGS =
	cuT( "layout( points ) in;\n"												)
	cuT( "layout( triangle_strip ) out;\n"										)
	cuT( "layout( max_vertices = 4 ) out;\n"									)
	cuT( "<matrix_buffer>"														)
	cuT( "<scene_buffer>"														)
	cuT( "<user_buffer>"														)
	cuT( "out vec2 geo_v2TexCoord;\n"											)
	cuT( "void main()\n"														)
	cuT( "{\n"																	)
	cuT( "	vec3 l_v3Pos = gl_in[0].gl_Position.xyz;\n"							)
	cuT( "	vec3 l_v3ToCamera = normalize( c3d_v3CameraPosition - l_v3Pos );\n"	)
	cuT( "	vec3 l_v3Up = vec3( 0.0, 1.0, 0.0 );\n"								)
	cuT( "	vec3 l_v3Right = cross( l_v3ToCamera, l_v3Up );\n"					)
 	cuT( "	l_v3Right.x *= float( c3d_v2iDimensions.x );\n"						)
	cuT( "	l_v3Right.z *= float( c3d_v2iDimensions.x );\n"						)
	cuT( "\n"																	)
	cuT( "	l_v3Pos -= (l_v3Right * 0.5);\n"									)
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n"	)
	cuT( "	geo_v2TexCoord = vec2( 0.0, 0.0 );\n"								)
	cuT( "	EmitVertex();\n"													)
	cuT( "\n"																	)
	cuT( "	l_v3Pos.y += float( c3d_v2iDimensions.y );\n"						)
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n"	)
	cuT( "	geo_v2TexCoord = vec2( 0.0, 1.0 );\n"								)
	cuT( "	EmitVertex();\n"													)
	cuT( "\n"																	)
	cuT( "	l_v3Pos.y -= float( c3d_v2iDimensions.y );\n"						)
	cuT( "	l_v3Pos += l_v3Right;\n"											)
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n"	)
	cuT( "	geo_v2TexCoord = vec2( 1.0, 0.0 );\n"								)
	cuT( "	EmitVertex();\n"													)
	cuT( "\n"																	)
	cuT( "	l_v3Pos.y += float( c3d_v2iDimensions.y );\n"						)
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n"	)
	cuT( "	geo_v2TexCoord = vec2( 1.0, 1.0 );\n"								)
	cuT( "	EmitVertex();\n"													)
	cuT( "\n"																	)
	cuT( "	EndPrimitive();\n"													)
	cuT( "}\n"																	);

static String BillboardPS =
	cuT( "uniform sampler2D c3d_mapDiffuse;\n"									)
	cuT( "in vec2 geo_v2TexCoord;\n"											)
	cuT( "out vec4 pxl_v4FragColor;\n"											)
	cuT( "void main()\n"														)
	cuT( "{\n"																	)
	cuT( "	pxl_v4FragColor = texture2D( c3d_mapDiffuse, geo_v2TexCoord );\n"	)
	cuT( "	if( pxl_v4FragColor.a < 0.5 )\n"									)
	cuT( "	{\n"																)
	cuT( "		discard;\n"														)
	cuT( "	}\n"																)
	cuT( "}\n"																	);

//*************************************************************************************************

GlBillboardList :: GlBillboardList( Castor3D::SceneRPtr p_pScene, GlRenderSystem * p_pRenderSystem, OpenGl & p_gl )
	:	BillboardList	( p_pScene, p_pRenderSystem	)
	,	m_gl			( p_gl						)
{
}

GlBillboardList :: ~GlBillboardList()
{
}

bool GlBillboardList :: DoInitialise()
{
	GLSL::ConstantsBase * l_pConstants;
	GLSLBillboard::VariablesBase * l_pVariables;
	GLSL::KeywordsBase * l_pKeywords;

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

	if( m_gl.HasUbo() )
	{
		l_pConstants = &GLSL::constantsUbo;
		l_pVariables = &GLSLBillboard::variablesUbo;
	}
	else
	{
		l_pConstants = &GLSL::constantsStd;
		l_pVariables = &GLSLBillboard::variablesStd;
	}

	String l_strVtxShader;
	l_strVtxShader += l_pKeywords->GetVersion();
	l_strVtxShader += l_pKeywords->GetAttribute( 0 )	+	cuT( "	vec4	vertex;\n" );
	l_strVtxShader += BillboardVS;

	String l_strGeoShader;
	l_strGeoShader += l_pKeywords->GetVersion();
	l_strGeoShader += BillboardGS;
	str_utils::replace( l_strGeoShader, cuT( "<matrix_buffer>"	), l_pConstants->Matrices()	);
	str_utils::replace( l_strGeoShader, cuT( "<scene_buffer>"	), l_pConstants->Scene()	);
	str_utils::replace( l_strGeoShader, cuT( "<user_buffer>"	), l_pVariables->User()		);

	String l_strPxlShader;
	l_strPxlShader += l_pKeywords->GetVersion();
	l_strPxlShader += BillboardPS;

	delete l_pKeywords;
	GLSL::ConstantsBase::Replace( l_strVtxShader );
	GLSL::ConstantsBase::Replace( l_strGeoShader );
	GLSL::ConstantsBase::Replace( l_strPxlShader );

	ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();

	m_pDimensionsUniform = std::static_pointer_cast< Point2iFrameVariable >( l_pProgram->GetUserBuffer()->CreateVariable( *l_pProgram.get(), eFRAME_VARIABLE_TYPE_VEC2I, cuT( "c3d_v2iDimensions" ) ) );
	l_pProgram->SetSource( eSHADER_TYPE_VERTEX,		eSHADER_MODEL_3, l_strVtxShader );
	l_pProgram->SetSource( eSHADER_TYPE_GEOMETRY,	eSHADER_MODEL_3, l_strGeoShader );
	l_pProgram->SetSource( eSHADER_TYPE_PIXEL,		eSHADER_MODEL_3, l_strPxlShader );
	l_pProgram->SetSource( eSHADER_TYPE_VERTEX,		eSHADER_MODEL_4, l_strVtxShader );
	l_pProgram->SetSource( eSHADER_TYPE_GEOMETRY,	eSHADER_MODEL_4, l_strGeoShader );
	l_pProgram->SetSource( eSHADER_TYPE_PIXEL,		eSHADER_MODEL_4, l_strPxlShader );
//	l_pProgram->Initialise();

	return true;//l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED;
}

//*************************************************************************************************
