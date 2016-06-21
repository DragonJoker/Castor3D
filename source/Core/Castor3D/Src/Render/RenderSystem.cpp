#include "RenderSystem.hpp"

#include "Engine.hpp"
#include "ShaderManager.hpp"

#include "Pipeline.hpp"
#include "Viewport.hpp"

#include "Event/Frame/CleanupEvent.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/OverlayRenderer.hpp"
#include "Plugin/Plugin.hpp"
#include "Mesh/Submesh.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/FrameVariable.hpp"
#include "Texture/Sampler.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderSystem::RenderSystem( Engine & p_engine, String const & p_name )
		: OwnedBy< Engine >{ p_engine }
		, m_name{ p_name }
		, m_initialised{ false }
		, m_currentContext{ nullptr }
		, m_pCurrentCamera{ nullptr }
		, m_gpuInformations{}
	{
	}

	RenderSystem::~RenderSystem()
	{
		m_mainContext.reset();
	}

	void RenderSystem::Initialise( GpuInformations && p_informations )
	{
		m_gpuInformations = std::move( p_informations );
		DoInitialise();
		m_gpuInformations.UpdateMaxShaderModel();
		REQUIRE( m_gpuInformations.GetMaxShaderModel() >= eSHADER_MODEL_3 );
		Logger::LogInfo( cuT( "Vendor: " ) + m_gpuInformations.GetVendor() );
		Logger::LogInfo( cuT( "Renderer: " ) + m_gpuInformations.GetRenderer() );
		Logger::LogInfo( cuT( "Version: " ) + m_gpuInformations.GetVersion() );
	}

	void RenderSystem::Cleanup()
	{
		if ( m_mainContext )
		{
			m_mainContext->Cleanup();
			m_mainContext.reset();
		}

		DoCleanup();

#if !defined( NDEBUG )

		DoReportTracked();

#endif
	}

	void RenderSystem::RenderAmbientLight( Castor::Colour const & p_clColour, FrameVariableBuffer & p_variableBuffer )
	{
		Point4fFrameVariableSPtr l_variable;
		p_variableBuffer.GetVariable( ShaderProgram::AmbientLight, l_variable );
		l_variable->SetValue( rgba_float( p_clColour ) );
	}

	void RenderSystem::PushScene( Scene * p_scene )
	{
		m_stackScenes.push( p_scene );
	}

	void RenderSystem::PopScene()
	{
		m_stackScenes.pop();
	}

	Scene * RenderSystem::GetTopScene()
	{
		Scene * l_return = nullptr;

		if ( m_stackScenes.size() )
		{
			l_return = m_stackScenes.top();
		}

		return l_return;
	}

	Camera * RenderSystem::GetCurrentCamera()const
	{
		return m_pCurrentCamera;
	}

	void RenderSystem::SetCurrentCamera( Camera * p_pCamera )
	{
		m_pCurrentCamera = p_pCamera;
	}

	GLSL::GlslWriter RenderSystem::CreateGlslWriter()
	{
		return GLSL::GlslWriter{ GLSL::GlslWriterConfig{ m_gpuInformations.GetShaderLanguageVersion(), m_gpuInformations.HasConstantsBuffers(), m_gpuInformations.HasTextureBuffers() } };
	}

	String RenderSystem::GetVertexShaderSource( uint32_t p_programFlags )
	{
		using namespace GLSL;
		auto l_writer = CreateGlslWriter();
		// Vertex inputs
		Vec4 position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
		Vec3 normal = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal );
		Vec3 tangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent );
		Vec3 bitangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent );
		Vec3 texture = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture );
		Optional< IVec4 > bone_ids0 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::Skinning ) );
		Optional< IVec4 > bone_ids1 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::Skinning ) );
		Optional< Vec4 > weights0 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::Skinning ) );
		Optional< Vec4 > weights1 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::Skinning ) );
		Optional< Mat4 > transform = l_writer.GetAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::Instantiation ) );
		Optional< Vec4 > position2 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::Morphing ) );
		Optional< Vec3 > normal2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::Morphing ) );
		Optional< Vec3 > tangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::Morphing ) );
		Optional< Vec3 > bitangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::Morphing ) );
		Optional< Vec3 > texture2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::Morphing ) );
		Optional< Float > time = l_writer.GetUniform< Float >( ShaderProgram::Time, CheckFlag( p_programFlags, ProgramFlag::Morphing ) );

		UBO_MATRIX( l_writer );

		// Outputs
		auto vtx_vertex = l_writer.GetOutput< Vec3 >( cuT( "vtx_vertex" ) );
		auto vtx_normal = l_writer.GetOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_time = l_writer.GetOutput< Float >( cuT( "vtx_time" ), CheckFlag( p_programFlags, ProgramFlag::Morphing ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Vertex, vec4( position.SWIZZLE_XYZ, 1.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normal, vec4( normal, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangent, vec4( tangent, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Bitangent, vec4( bitangent, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Texture, texture );
			auto l_mtxModel = l_writer.GetLocale< Mat4 >( cuT( "l_mtxModel" ) );
			bool l_set = false;

			if ( CheckFlag( p_programFlags, ProgramFlag::Skinning ) )
			{
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxBoneTransform, c3d_mtxBones[bone_ids0[Int( 0 )]] * weights0[Int( 0 )] );
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[Int( 1 )]] * weights0[Int( 1 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[Int( 2 )]] * weights0[Int( 2 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[Int( 3 )]] * weights0[Int( 3 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 0 )]] * weights1[Int( 0 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 1 )]] * weights1[Int( 1 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 2 )]] * weights1[Int( 2 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[Int( 3 )]] * weights1[Int( 3 )];
				l_mtxModel = l_mtxBoneTransform;
				l_set = true;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::Morphing ) )
			{
				LOCALE_ASSIGN( l_writer, Float, l_time, Float( 1.0 ) - time );
				l_v4Vertex = vec4( l_v4Vertex.SWIZZLE_XYZ * l_time + position2.SWIZZLE_XYZ * time, 1.0 );
				l_v4Normal = vec4( l_v4Normal.SWIZZLE_XYZ * l_time + normal2.SWIZZLE_XYZ * time, 1.0 );
				l_v4Tangent = vec4( l_v4Tangent.SWIZZLE_XYZ * l_time + tangent2.SWIZZLE_XYZ * time, 1.0 );
				l_v4Bitangent = vec4( l_v4Bitangent.SWIZZLE_XYZ * l_time + bitangent2.SWIZZLE_XYZ * time, 1.0 );
				l_v3Texture = l_v3Texture * l_writer.Paren( Float( 1.0 ) - time ) + texture2 * time;
				vtx_time = time;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::Instantiation ) )
			{
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxMV, transform );
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxN, transpose( inverse( l_mtxMV ) ) );

				if ( l_set )
				{
					l_mtxModel = l_mtxMV * l_mtxModel;
				}
				else
				{
					l_mtxModel = l_mtxMV;
				}
			}
			else
			{
				if ( l_set )
				{
					l_mtxModel = c3d_mtxModel * l_mtxModel;
				}
				else
				{
					l_mtxModel = c3d_mtxModel;
				}
			}

			vtx_texture = l_v3Texture;
			vtx_vertex = l_writer.Paren( l_mtxModel * l_v4Vertex ).SWIZZLE_XYZ;
			vtx_normal = normalize( l_writer.Paren( l_mtxModel * l_v4Normal ).SWIZZLE_XYZ );
			vtx_tangent = normalize( l_writer.Paren( l_mtxModel * l_v4Tangent ).SWIZZLE_XYZ );
			vtx_bitangent = normalize( l_writer.Paren( l_mtxModel * l_v4Bitangent ).SWIZZLE_XYZ );
			gl_Position = c3d_mtxProjection * c3d_mtxView * l_mtxModel * l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	ShaderProgramSPtr RenderSystem::CreateBillboardsProgram( RenderTechnique const & p_technique, uint32_t p_flags )
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

		eTOPOLOGY l_input = eTOPOLOGY_POINTS;
		eTOPOLOGY l_output = eTOPOLOGY_TRIANGLE_STRIPS;
		uint32_t l_count = 4;

		ShaderManager & l_manager = GetEngine()->GetShaderManager();
		ShaderProgramSPtr l_program = l_manager.GetNewProgram();
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
		l_manager.CreateSceneBuffer( *l_program, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
		l_manager.CreatePassBuffer( *l_program, MASK_SHADER_TYPE_PIXEL );
		l_manager.CreateTextureVariables( *l_program, p_flags );
		FrameVariableBufferSPtr l_billboardUbo = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "Billboard" ) );
		l_program->AddFrameVariableBuffer( l_billboardUbo, MASK_SHADER_TYPE_GEOMETRY );

		ShaderObjectSPtr l_object = l_program->CreateObject( eSHADER_TYPE_GEOMETRY );
		l_object->SetInputType( l_input );
		l_object->SetOutputType( l_output );
		l_object->SetOutputVtxCount( l_count );

		String l_strVtxShader;
		{
			auto l_writer = CreateGlslWriter();

			// Shader inputs
			Vec4 position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );

			// Shader outputs
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = vec4( position.SWIZZLE_XYZ, 1.0 );
			} );

			l_strVtxShader = l_writer.Finalise();
		}

		String l_strGeoShader;
		{
			auto l_writer = CreateGlslWriter();

			l_writer.InputGeometryLayout( PRIMITIVES[l_input] );
			l_writer.OutputGeometryLayout( PRIMITIVES[l_output] );
			l_writer.OutputVertexCount( l_count );

			// Shader inputs
			UBO_MATRIX( l_writer );
			UBO_SCENE( l_writer );
			UBO_BILLBOARD( l_writer );
			auto gl_in = l_writer.GetBuiltin< gl_PerVertex >( cuT( "gl_in" ), 8u );

			// Shader outputs
			auto vtx_vertex = l_writer.GetOutput< Vec3 >( cuT( "vtx_vertex" ) );
			auto vtx_normal = l_writer.GetOutput< Vec3 >( cuT( "vtx_normal" ) );
			auto vtx_tangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_tangent" ) );
			auto vtx_bitangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_bitangent" ) );
			auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxVP, c3d_mtxProjection * c3d_mtxView );

				LOCALE_ASSIGN( l_writer, Vec3, l_pos, gl_in[0].gl_Position().SWIZZLE_XYZ );
				LOCALE_ASSIGN( l_writer, Vec3, l_toCamera, normalize( vec3( c3d_v3CameraPosition.SWIZZLE_X, c3d_v3CameraPosition.SWIZZLE_Y, c3d_v3CameraPosition.SWIZZLE_Z ) - l_pos ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_up, vec3( Float( 0 ), 1.0, 0.0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_left, cross( l_toCamera, l_up ) );

				LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( l_toCamera.SWIZZLE_X, 0.0, l_toCamera.SWIZZLE_Z ) ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, l_up );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, l_left );

				l_left *= c3d_v2iDimensions.SWIZZLE_X;
				l_up *= c3d_v2iDimensions.SWIZZLE_Y;
				l_writer << Endl();

				{
					l_pos -= ( l_left * 0.5 );
					vtx_vertex = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).SWIZZLE_XYZ;
					gl_Position = l_mtxVP * vec4( vtx_vertex, 1.0 );
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = vec3( Float( 1.0 ), 0.0, 0.0 );
					l_writer.EmitVertex();
				}
				l_writer << Endl();

				{
					l_pos += l_up;
					vtx_vertex = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).SWIZZLE_XYZ;
					gl_Position = l_mtxVP * vec4( vtx_vertex, 1.0 );
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = vec3( Float( 1.0 ), 1.0, 0.0 );
					l_writer.EmitVertex();
				}
				l_writer << Endl();

				{
					l_pos -= l_up;
					l_pos += l_left;
					vtx_vertex = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).SWIZZLE_XYZ;
					gl_Position = l_mtxVP * vec4( vtx_vertex, 1.0 );
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = vec3( Float( 0.0 ), 0.0, 0.0 );
					l_writer.EmitVertex();
				}
				l_writer << Endl();

				{
					l_pos += l_up;
					vtx_vertex = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).SWIZZLE_XYZ;
					gl_Position = l_mtxVP * vec4( vtx_vertex, 1.0 );
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = vec3( Float( 0.0 ), 1.0, 0.0 );
					l_writer.EmitVertex();
				}
				l_writer << Endl();
				l_writer.EndPrimitive();
			} );

			l_strGeoShader = l_writer.Finalise();
		}

		String l_strPxlShader = p_technique.GetPixelShaderSource( p_flags );

		std::static_pointer_cast< Point2iFrameVariable >( l_billboardUbo->CreateVariable( *l_program.get(), FrameVariableType::Vec2i, cuT( "c3d_v2iDimensions" ) ) );
		l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_3, l_strVtxShader );
		l_program->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_3, l_strGeoShader );
		l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_3, l_strPxlShader );
		l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_4, l_strVtxShader );
		l_program->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_4, l_strGeoShader );
		l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_4, l_strPxlShader );

		return l_program;
	}

#if C3D_TRACE_OBJECTS

	void RenderSystem::DoReportTracked()
	{
		for ( auto const & l_decl : m_allocated )
		{
			if ( l_decl.m_ref > 0 )
			{
				std::stringstream l_stream;
				l_stream << "Leaked 0x" << std::hex << l_decl.m_object << std::dec << " (" << l_decl.m_name << "), from file " << l_decl.m_file << ", line " << l_decl.m_line << std::endl;
				l_stream << string::string_cast< char >( l_decl.m_stack ) << std::endl;
				Castor::Logger::LogError( l_stream.str() );
			}
		}
	}

#endif

}
