#include "GlBillboardList.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderObject.hpp"
#include "GlPixelShaderSource.hpp"

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
	cuT( "    gl_Position = vec4( vertex.xyz, 1.0 );\n" )
	cuT( "}\n" );

static String BillboardGS =
	cuT( "layout( <in_primitives> ) in;\n" )
	cuT( "layout( <out_primitives> ) out;\n" )
	cuT( "layout( max_vertices = <max_vertices> ) out;\n" )
	cuT( "\n" )
	cuT( "<matrix_buffer>" )
	cuT( "<scene_buffer>" )
	cuT( "<billboard_buffer>" )
	cuT( "\n" )
	cuT( "out vec3 vtx_normal;\n" )
	cuT( "out vec3 vtx_tangent;\n" )
	cuT( "out vec3 vtx_bitangent;\n" )
	cuT( "out vec3 vtx_texture;\n" )
	cuT( "\n" )
	cuT( "void main()\n" )
	cuT( "{\n" )
	cuT( "    vec3 l_position = ( c3d_mtxProjectionModelView * gl_in[0].gl_Position ).xyz;\n" )
	cuT( "    l_position.y = c3d_v3CameraPosition.y;\n" )
	cuT( "    vec3 l_toCamera = c3d_v3CameraPosition - l_position;\n" )
	cuT( "    vec3 l_up = vec3( 0.0, 1.0, 0.0 );\n" )
	cuT( "    vec3 l_right = cross( l_toCamera, l_up );\n" )
	cuT( "    vec3 l_v3Normal = ( c3d_mtxProjectionModelView * vec4( 0.0, 0.0, -1.0, 0.0 ) ).xyz;\n" )
	cuT( "    l_v3Normal = ( c3d_mtxProjectionModelView * vec4( l_v3Normal, 0.0 ) ).xyz;\n" )
	cuT( "    \n" )
	cuT( "    vec3 l_position0 = l_position - ( l_right * 0.5 );\n" )
	cuT( "    vec3 l_v2Texture0 = vec3( 0.0, 0.0, 0.0 );\n" )
	cuT( "    vec3 l_position1 = l_position0 + vec3( 0.0, float( c3d_v2iDimensions.y ), 0.0 );\n" )
	cuT( "    vec3 l_v2Texture1 = vec3( 0.0, 1.0, 0.0 );\n" )
	cuT( "    vec3 l_position2 = l_position0 + l_right;\n" )
	cuT( "    vec3 l_v2Texture2 = vec3( 1.0, 0.0, 0.0 );\n" )
	cuT( "    vec3 l_position3 = l_position2 + vec3( 0.0, float( c3d_v2iDimensions.y ), 0.0 );\n" )
	cuT( "    vec3 l_v2Texture3 = vec3( 1.0, 1.0, 0.0 );\n" )
	cuT( "    \n" )
	cuT( "    vec3 l_vec2m1 = l_position1 - l_position0;\n" )
	cuT( "    vec3 l_vec3m1 = l_position2 - l_position0;\n" )
	cuT( "    vec3 l_tex2m1 = l_v2Texture1 - l_v2Texture0;\n" )
	cuT( "    vec3 l_tex3m1 = l_v2Texture2 - l_v2Texture0;\n" )
	cuT( "    vec3 l_v3Tangent = normalize( ( l_vec2m1 * l_tex3m1.x ) - ( l_vec3m1 * l_tex2m1.y ) );\n" )
	cuT( "    vec3 l_v3Bitangent = cross( l_v3Tangent, l_v3Normal );\n" )
	cuT( "    \n" )
	cuT( "    {\n" )
	cuT( "        gl_Position = vec4( l_position0, 1.0 );\n" )
	cuT( "        vtx_normal = l_v3Normal;\n" )
	cuT( "        vtx_tangent = l_v3Tangent;\n" )
	cuT( "        vtx_bitangent = l_v3Bitangent;\n" )
	cuT( "        vtx_texture = l_v2Texture0;\n" )
	cuT( "        EmitVertex();\n" )
	cuT( "    }\n" )
	cuT( "    \n" )
	cuT( "    {\n" )
	cuT( "        gl_Position = vec4( l_position1, 1.0 );\n" )
	cuT( "        vtx_normal = l_v3Normal;\n" )
	cuT( "        vtx_tangent = l_v3Tangent;\n" )
	cuT( "        vtx_bitangent = l_v3Bitangent;\n" )
	cuT( "        vtx_texture = l_v2Texture1;\n" )
	cuT( "        EmitVertex();\n" )
	cuT( "    }\n" )
	cuT( "    \n" )
	cuT( "    {\n" )
	cuT( "        gl_Position = vec4( l_position2, 1.0 );\n" )
	cuT( "        vtx_normal = l_v3Normal;\n" )
	cuT( "        vtx_tangent = l_v3Tangent;\n" )
	cuT( "        vtx_bitangent = l_v3Bitangent;\n" )
	cuT( "        vtx_texture = l_v2Texture2;\n" )
	cuT( "        EmitVertex();\n" )
	cuT( "    }\n" )
	cuT( "    \n" )
	cuT( "    {\n" )
	cuT( "        gl_Position = vec4( l_position3, 1.0 );\n" )
	cuT( "        vtx_normal = l_v3Normal;\n" )
	cuT( "        vtx_tangent = l_v3Tangent;\n" )
	cuT( "        vtx_bitangent = l_v3Bitangent;\n" )
	cuT( "        vtx_texture = l_v2Texture3;\n" )
	cuT( "        EmitVertex();\n" )
	cuT( "    }\n" )
	cuT( "    \n" )
	cuT( "    EndPrimitive();\n" )
	cuT( "}\n" );

//*************************************************************************************************

GlBillboardList::GlBillboardList( Castor3D::SceneSPtr p_pScene, GlRenderSystem * p_pRenderSystem, OpenGl & p_gl )
	: BillboardList( p_pScene, p_pRenderSystem )
	, m_gl( p_gl )
{
}

GlBillboardList::~GlBillboardList()
{
}

ShaderProgramBaseSPtr GlBillboardList::DoGetProgram( uint32_t p_flags )
{
	static String PRIMITIVES[] =
	{
		cuT( "points" ),//eTOPOLOGY_POINTS
		cuT( "lines" ),//eTOPOLOGY_LINES
		cuT( "line_loop" ),//eTOPOLOGY_LINE_LOOP
		cuT( "line_strip" ),//eTOPOLOGY_LINE_STRIP
		cuT( "triangles" ),//eTOPOLOGY_TRIANGLES
		cuT( "triangle_strip" ),//eTOPOLOGY_TRIANGLE_STRIPS
		cuT( "triangle_fan" ),//eTOPOLOGY_TRIANGLE_FAN
		cuT( "quads" ),//eTOPOLOGY_QUADS
		cuT( "quad_strip" ),//eTOPOLOGY_QUAD_STRIPS
		cuT( "polygon" ),//eTOPOLOGY_POLYGON
	};

	ShaderManager & l_manager = m_pRenderSystem->GetEngine()->GetShaderManager();
	ShaderProgramBaseSPtr l_pProgram = l_manager.GetNewProgram();
	l_manager.CreateMatrixBuffer( *l_pProgram, MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
	l_manager.CreateSceneBuffer( *l_pProgram, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
	l_manager.CreatePassBuffer( *l_pProgram, MASK_SHADER_TYPE_PIXEL );
	l_manager.CreateTextureVariables( *l_pProgram, p_flags );
	FrameVariableBufferSPtr l_billboardUbo = m_pRenderSystem->CreateFrameVariableBuffer( cuT( "Billboard" ) );
	l_pProgram->AddFrameVariableBuffer( l_billboardUbo, MASK_SHADER_TYPE_GEOMETRY );

	ShaderObjectBaseSPtr l_pObject = l_pProgram->CreateObject( eSHADER_TYPE_GEOMETRY );
	l_pObject->SetInputType( eTOPOLOGY_POINTS );
	l_pObject->SetOutputType( eTOPOLOGY_TRIANGLE_STRIPS );
	l_pObject->SetOutputVtxCount( 4 );
	
	GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
	GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
	std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );
	String l_strVersion = l_pKeywords->GetVersion();
	String l_strAttribute0 = l_pKeywords->GetAttribute( 0 );
	String l_strAttribute1 = l_pKeywords->GetAttribute( 1 );
	String l_strAttribute2 = l_pKeywords->GetAttribute( 2 );
	String l_strAttribute3 = l_pKeywords->GetAttribute( 3 );
	String l_strAttribute4 = l_pKeywords->GetAttribute( 4 );
	String l_strAttribute5 = l_pKeywords->GetAttribute( 5 );
	String l_strAttribute6 = l_pKeywords->GetAttribute( 6 );
	String l_strAttribute7 = l_pKeywords->GetAttribute( 7 );
	String l_strIn = l_pKeywords->GetIn();
	String l_strOut = l_pKeywords->GetOut();

	String l_strVtxShader;
	l_strVtxShader += l_strVersion;
	l_strVtxShader += l_strAttribute0 + cuT( "	<vec4> vertex;\n" );
	l_strVtxShader += BillboardVS;
	str_utils::replace( l_strVtxShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	GLSL::ConstantsBase::Replace( l_strVtxShader );

	String l_strGeoShader;
	l_strGeoShader += l_strVersion;
	l_strGeoShader += BillboardGS;
	str_utils::replace( l_strGeoShader, cuT( "<matrix_buffer>" ), l_pConstants->Matrices() );
	str_utils::replace( l_strGeoShader, cuT( "<scene_buffer>" ), l_pConstants->Scene() );
	str_utils::replace( l_strGeoShader, cuT( "<billboard_buffer>" ), l_pConstants->Billboard() );
	str_utils::replace( l_strGeoShader, cuT( "<in_primitives>" ), PRIMITIVES[l_pObject->GetInputType()] );
	str_utils::replace( l_strGeoShader, cuT( "<out_primitives>" ), PRIMITIVES[l_pObject->GetOutputType()] );
	str_utils::replace( l_strGeoShader, cuT( "<max_vertices>" ), str_utils::to_string( l_pObject->GetOutputVtxCount() ) );
	str_utils::replace( l_strGeoShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	GLSL::ConstantsBase::Replace( l_strGeoShader );

	String l_strPxlShader = l_pProgram->GetPixelShaderSource( p_flags );

	m_pDimensionsUniform = std::static_pointer_cast< Point2iFrameVariable >( l_billboardUbo->CreateVariable( *l_pProgram.get(), eFRAME_VARIABLE_TYPE_VEC2I, cuT( "c3d_v2iDimensions" ) ) );
	l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_3, l_strVtxShader );
	l_pProgram->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_3, l_strGeoShader );
	l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_3, l_strPxlShader );
	l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_4, l_strVtxShader );
	l_pProgram->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_4, l_strGeoShader );
	l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_4, l_strPxlShader );

	return l_pProgram;
}

bool GlBillboardList::DoInitialise()
{
	bool l_return = false;
	ShaderProgramBaseSPtr l_program = m_wpProgram.lock();

	if ( l_program )
	{
		l_program->Initialise();
	}

	return true;
}

//*************************************************************************************************
