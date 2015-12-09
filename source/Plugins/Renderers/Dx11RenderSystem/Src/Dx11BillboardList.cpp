#include "Dx11BillboardList.hpp"
#include "Dx11RenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;
using namespace Dx11Render;

//*************************************************************************************************

DxBillboardList::DxBillboardList( Castor3D::SceneSPtr p_scene, DxRenderSystem & p_renderSystem )
	: BillboardList( p_scene, p_renderSystem )
{
}

DxBillboardList::~DxBillboardList()
{
}

bool DxBillboardList::DoInitialise()
{
	return true;
}

ShaderProgramBaseSPtr DxBillboardList::DoGetProgram( RenderTechniqueBase const & p_technique, uint32_t p_flags )
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

	ShaderProgramBaseSPtr l_program = GetOwner()->GetOwner()->GetShaderManager().GetNewProgram();

	//l_program->CreateFrameVariable( ShaderProgramBase::MapAmbient, eSHADER_TYPE_PIXEL );
	//l_program->CreateFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL );
	//l_program->CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
	//l_program->CreateFrameVariable( ShaderProgramBase::MapNormal, eSHADER_TYPE_PIXEL );
	//l_program->CreateFrameVariable( ShaderProgramBase::MapSpecular, eSHADER_TYPE_PIXEL );
	//l_program->CreateFrameVariable( ShaderProgramBase::MapOpacity, eSHADER_TYPE_PIXEL );
	//l_program->CreateFrameVariable( ShaderProgramBase::MapGloss, eSHADER_TYPE_PIXEL );
	//l_program->CreateFrameVariable( ShaderProgramBase::MapHeight, eSHADER_TYPE_PIXEL );

	//ShaderObjectBaseSPtr l_object = l_program->CreateObject( eSHADER_TYPE_GEOMETRY );
	//l_object->SetInputType( eTOPOLOGY_POINTS );
	//l_object->SetOutputType( eTOPOLOGY_TRIANGLE_STRIPS );
	//l_object->SetOutputVtxCount( 4 );
	//
	//GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( GetOpenGl() );
	//GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( GetOpenGl() );
	//std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( GetOpenGl() );
	//String l_strVersion = l_pKeywords->GetVersion();
	//String l_strAttribute0 = l_pKeywords->GetAttribute( 0 );
	//String l_strAttribute1 = l_pKeywords->GetAttribute( 1 );
	//String l_strAttribute2 = l_pKeywords->GetAttribute( 2 );
	//String l_strAttribute3 = l_pKeywords->GetAttribute( 3 );
	//String l_strAttribute4 = l_pKeywords->GetAttribute( 4 );
	//String l_strAttribute5 = l_pKeywords->GetAttribute( 5 );
	//String l_strAttribute6 = l_pKeywords->GetAttribute( 6 );
	//String l_strAttribute7 = l_pKeywords->GetAttribute( 7 );
	//String l_strIn = l_pKeywords->GetIn();
	//String l_strOut = l_pKeywords->GetOut();

	//String l_strVtxShader;
	//l_strVtxShader += l_strVersion;
	//l_strVtxShader += l_strAttribute0 + cuT( "    <vec4>   vertex;\n" );
	//l_strVtxShader += BillboardVS;
	//string::replace( l_strVtxShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	//GLSL::ConstantsBase::Replace( l_strVtxShader );

	//String l_strGeoShader;
	//l_strGeoShader += l_strVersion;
	//l_strGeoShader += BillboardGS;
	//string::replace( l_strGeoShader, cuT( "<matrix_buffer>" ), l_pConstants->Matrices() );
	//string::replace( l_strGeoShader, cuT( "<scene_buffer>" ), l_pConstants->Scene() );
	//string::replace( l_strGeoShader, cuT( "<user_buffer>" ), l_pConstants->User() );
	//string::replace( l_strGeoShader, cuT( "<in_primitives>" ), PRIMITIVES[l_object->GetInputType()] );
	//string::replace( l_strGeoShader, cuT( "<out_primitives>" ), PRIMITIVES[l_object->GetOutputType()] );
	//string::replace( l_strGeoShader, cuT( "<max_vertices>" ), string::to_string( l_object->GetOutputVtxCount() ) );
	//string::replace( l_strGeoShader, cuT( "<layout>" ), l_pKeywords->GetLayout() );
	//GLSL::ConstantsBase::Replace( l_strGeoShader );

	//String l_strPxlShader = l_program->GetPixelShaderSource( p_flags );

	//m_pDimensionsUniform = std::static_pointer_cast< Point2iFrameVariable >( l_program->GetUserBuffer()->CreateVariable( *l_program.get(), eFRAME_VARIABLE_TYPE_VEC2I, cuT( "c3d_v2iDimensions" ) ) );
	//l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_3, l_strVtxShader );
	//l_program->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_3, l_strGeoShader );
	//l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_3, l_strPxlShader );
	//l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_4, l_strVtxShader );
	//l_program->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_4, l_strGeoShader );
	//l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_4, l_strPxlShader );

	return l_program;
}

//*************************************************************************************************
