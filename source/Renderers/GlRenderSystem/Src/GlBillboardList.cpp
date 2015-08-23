#include "GlBillboardList.hpp"

#include "GlRenderSystem.hpp"
#include "GlShaderObject.hpp"
#include "GlShaderSource.hpp"

#include <FrameVariableBuffer.hpp>
#include <OneFrameVariable.hpp>
#include <PointFrameVariable.hpp>
#include <MatrixFrameVariable.hpp>
#include <ShaderProgram.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GlRender;

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
	using namespace GLSL;

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

	String l_strVtxShader;
	{
		GlslWriter l_writer( m_gl, eSHADER_TYPE_VERTEX );
		l_writer << Version() << Endl();

		// Shader inputs
		ATTRIBUTE( l_writer, IVec4, vertex );

		l_writer.Implement_Function< void >( cuT( "main" ), [&]()
		{
			BUILTIN( l_writer, Vec4, gl_Position ) = vec4( vertex.xyz(), 1.0 );
		} );

		l_strVtxShader = l_writer.Finalise();
	}

	String l_strGeoShader;
	{
		GlslWriter l_writer( m_gl, eSHADER_TYPE_VERTEX );
		l_writer << Version() << Endl();

		l_writer << cuT( "layout( " ) << PRIMITIVES[l_pObject->GetInputType()] << cuT( " ) in;" );
		l_writer << cuT( "layout( " ) << PRIMITIVES[l_pObject->GetOutputType()] << cuT( " ) out;" );
		l_writer << cuT( "layout( max_vertices = " ) << PRIMITIVES[l_pObject->GetOutputVtxCount()] << cuT( " ) out;" );

		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_BILLBOARD( l_writer );

		OUT( l_writer, Vec3, vtx_normal );
		OUT( l_writer, Vec3, vtx_tangent );
		OUT( l_writer, Vec3, vtx_bitangent );
		OUT( l_writer, Vec3, vtx_texture );

		BUILTIN( l_writer, Vec4, gl_Position );
		BUILTIN_ARRAY( l_writer, gl_PerVertex, gl_in, 8 );

		l_writer.Implement_Function< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec3, l_position, ( c3d_mtxProjectionModelView * gl_in[0].gl_Position() ).xyz() );
			l_position.y() = c3d_v3CameraPosition.y();
			LOCALE_ASSIGN( l_writer, Vec3, l_toCamera, c3d_v3CameraPosition - l_position );
			LOCALE_ASSIGN( l_writer, Vec3, l_up, vec3( Float( 0.0f ), 1.0, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_right, cross( l_toCamera, l_up ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, ( c3d_mtxProjectionModelView * vec4( Float( 0.0f ), 0.0, -1.0, 0.0 ) ).xyz() );
			l_v3Normal = ( c3d_mtxProjectionModelView * vec4( l_v3Normal, 0.0 ) ).xyz();
			
			LOCALE_ASSIGN( l_writer, Vec3, l_position0, l_position - ( l_right * 0.5 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture0, vec3( Float( 0.0f ), 0.0, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_position1, l_position0 + vec3( Float( 0.0f ), CAST( l_writer, Float, c3d_v2iDimensions.y() ), 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture1, vec3( Float( 0.0f ), 1.0, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_position2, l_position0 + l_right );
			LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture2, vec3( Float( 1.0f ), 0.0, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_position3, l_position2 + vec3( Float( 0.0f ), CAST( l_writer, Float, c3d_v2iDimensions.y() ), 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture3, vec3( Float( 1.0f ), 1.0, 0.0 ) );
			
			LOCALE_ASSIGN( l_writer, Vec3, l_vec2m1, l_position1 - l_position0 );
			LOCALE_ASSIGN( l_writer, Vec3, l_vec3m1, l_position2 - l_position0 );
			LOCALE_ASSIGN( l_writer, Vec3, l_tex2m1, l_v2Texture1 - l_v2Texture0 );
			LOCALE_ASSIGN( l_writer, Vec3, l_tex3m1, l_v2Texture2 - l_v2Texture0 );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, normalize( ( l_vec2m1 * l_tex3m1.x() ) - ( l_vec3m1 * l_tex2m1.y() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, cross( l_v3Tangent, l_v3Normal ) );

			{
				IndentBlock l_block( l_writer );
				l_writer << Endl();
				gl_Position = vec4( l_position0, 1.0 );
				vtx_normal = l_v3Normal;
				vtx_tangent = l_v3Tangent;
				vtx_bitangent = l_v3Bitangent;
				vtx_texture = l_v2Texture0;
				l_writer.EmitVertex();
			}
			l_writer << Endl();
			
			{
				IndentBlock l_block( l_writer );
				l_writer << Endl();
				gl_Position = vec4( l_position1, 1.0 );
				vtx_normal = l_v3Normal;
				vtx_tangent = l_v3Tangent;
				vtx_bitangent = l_v3Bitangent;
				vtx_texture = l_v2Texture1;
				l_writer.EmitVertex();
			}
			l_writer << Endl();
			
			{
				IndentBlock l_block( l_writer );
				l_writer << Endl();
				gl_Position = vec4( l_position2, 1.0 );
				vtx_normal = l_v3Normal;
				vtx_tangent = l_v3Tangent;
				vtx_bitangent = l_v3Bitangent;
				vtx_texture = l_v2Texture2;
				l_writer.EmitVertex();
			}
			l_writer << Endl();
			
			{
				gl_Position = vec4( l_position3, 1.0 );
				l_writer << Endl();
				vtx_normal = l_v3Normal;
				vtx_tangent = l_v3Tangent;
				vtx_bitangent = l_v3Bitangent;
				vtx_texture = l_v2Texture3;
				l_writer.EmitVertex();
			}
			l_writer << Endl();
			l_writer.EndPrimitive();
		} );

		l_strVtxShader = l_writer.Finalise();
	}

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
