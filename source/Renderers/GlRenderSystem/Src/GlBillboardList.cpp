#include "GlBillboardList.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderObject.hpp"

#include <FrameVariableBuffer.hpp>
#include <OneFrameVariable.hpp>
#include <PointFrameVariable.hpp>
#include <MatrixFrameVariable.hpp>
#include <ShaderProgram.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GlRender;

//*************************************************************************************************

static String BillboardVS =
	cuT( "void main()\n" )
	cuT( "{\n" )
	cuT( "	gl_Position = vec4( vertex, 1.0 );\n" )
	cuT( "}\n" );

static String BillboardGS =
	cuT( "layout( points ) in;\n" )
	cuT( "layout( triangle_strip ) out;\n" )
	cuT( "layout( max_vertices = 4 ) out;\n" )
	cuT( "<matrix_buffer>" )
	cuT( "<scene_buffer>" )
	cuT( "<user_buffer>" )
	cuT( "out vec2 geo_v2TexCoord;\n" )
	cuT( "void main()\n" )
	cuT( "{\n" )
	cuT( "	vec3 l_v3Pos = gl_in[0].gl_Position.xyz;\n" )
	cuT( "	vec3 l_v3Vertex = ( c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 ) ).xyz;\n" )
	cuT( "	vec3 l_v3ToCamera = normalize( c3d_v3CameraPosition - l_v3Pos );\n" )
	cuT( "	vec3 l_v3Up = vec3( 0.0, 1.0, 0.0 );\n" )
	cuT( "	vec3 l_v3Right = cross( l_v3ToCamera, l_v3Up );\n" )
	cuT( "	l_v3Right.x *= float( c3d_v2iDimensions.x );\n" )
	cuT( "	l_v3Right.z *= float( c3d_v2iDimensions.x );\n" )
	cuT( "\n" )
	cuT( "	l_v3Pos -= (l_v3Right * 0.5);\n" )
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n" )
	cuT( "	geo_v2TexCoord = vec2( 0.0, 0.0 );\n" )
	cuT( "	EmitVertex();\n" )
	cuT( "\n" )
	cuT( "	l_v3Pos.y += float( c3d_v2iDimensions.y );\n" )
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n" )
	cuT( "	geo_v2TexCoord = vec2( 0.0, 1.0 );\n" )
	cuT( "	EmitVertex();\n" )
	cuT( "\n" )
	cuT( "	l_v3Pos.y -= float( c3d_v2iDimensions.y );\n" )
	cuT( "	l_v3Pos += l_v3Right;\n" )
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n" )
	cuT( "	geo_v2TexCoord = vec2( 1.0, 0.0 );\n" )
	cuT( "	EmitVertex();\n" )
	cuT( "\n" )
	cuT( "	l_v3Pos.y += float( c3d_v2iDimensions.y );\n" )
	cuT( "	gl_Position = c3d_mtxProjectionView * vec4( l_v3Pos, 1.0 );\n" )
	cuT( "	geo_v2TexCoord = vec2( 1.0, 1.0 );\n" )
	cuT( "	EmitVertex();\n" )
	cuT( "\n" )
	cuT( "	EndPrimitive();\n" )
	cuT( "}\n" );

static String BillboardPS =
	cuT( "uniform sampler2D c3d_mapDiffuse;\n" )
	cuT( "in vec2 geo_v2TexCoord;\n" )
	cuT( "out vec4 pxl_v4FragColor;\n" )
	cuT( "void main()\n" )
	cuT( "{\n" )
	cuT( "	pxl_v4FragColor = <texture2D>( c3d_mapDiffuse, geo_v2TexCoord );\n" )
	cuT( "	if( pxl_v4FragColor.a < 0.5 )\n" )
	cuT( "	{\n" )
	cuT( "		discard;\n" )
	cuT( "	}\n" )
	cuT( "}\n" );

//*************************************************************************************************

GlBillboardList::GlBillboardList( Castor3D::SceneSPtr p_pScene, GlRenderSystem * p_pRenderSystem, OpenGl & p_gl )
	:	BillboardList( p_pScene, p_pRenderSystem )
	,	m_gl( p_gl	)
{
}

GlBillboardList::~GlBillboardList()
{
}

bool GlBillboardList::DoInitialise()
{
	GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
	GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
	std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

	String l_strVtxShader;
	l_strVtxShader += l_pKeywords->GetVersion();
	l_strVtxShader += l_pKeywords->GetAttribute( 0 )	+	cuT( "	vec4	vertex;\n" );
	l_strVtxShader += BillboardVS;
	str_utils::replace( l_strVtxShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	GLSL::ConstantsBase::Replace( l_strVtxShader );

	String l_strGeoShader;
	l_strGeoShader += l_pKeywords->GetVersion();
	l_strGeoShader += BillboardGS;
	str_utils::replace( l_strGeoShader, cuT( "<matrix_buffer>"	), l_pConstants->Matrices() );
	str_utils::replace( l_strGeoShader, cuT( "<scene_buffer>"	), l_pConstants->Scene() );
	str_utils::replace( l_strGeoShader, cuT( "<user_buffer>"	), l_pConstants->User() );
	str_utils::replace( l_strGeoShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	GLSL::ConstantsBase::Replace( l_strGeoShader );

	String l_strPxlShader;
	l_strPxlShader += l_pKeywords->GetVersion();
	l_strPxlShader += BillboardPS;
	str_utils::replace( l_strPxlShader, cuT( "<texture2D>" ), l_pKeywords->GetTexture2D() );
	str_utils::replace( l_strPxlShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
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
