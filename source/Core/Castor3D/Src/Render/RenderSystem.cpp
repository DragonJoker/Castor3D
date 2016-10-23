#include "RenderSystem.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

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
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderSystem::RenderSystem( Engine & p_engine, String const & p_name )
		: OwnedBy< Engine >{ p_engine }
		, m_name{ p_name }
		, m_initialised{ false }
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
		REQUIRE( m_gpuInformations.GetMaxShaderModel() >= ShaderModel::Model3 );
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

	GLSL::GlslWriter RenderSystem::CreateGlslWriter()
	{
		return GLSL::GlslWriter{ GLSL::GlslWriterConfig{ m_gpuInformations.GetShaderLanguageVersion(), m_gpuInformations.HasConstantsBuffers(), m_gpuInformations.HasTextureBuffers() } };
	}

	ShaderProgramSPtr RenderSystem::CreateBillboardsProgram( RenderPass const & p_renderPass, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		Topology l_input = Topology::Points;
		Topology l_output = Topology::TriangleStrips;
		uint32_t l_count = 4;

		auto & l_cache = GetEngine()->GetShaderProgramCache();
		ShaderProgramSPtr l_program = l_cache.GetNewProgram();
		l_cache.CreateMatrixBuffer( *l_program, p_programFlags, MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
		l_cache.CreateSceneBuffer( *l_program, p_programFlags, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
		l_cache.CreatePassBuffer( *l_program, p_programFlags, MASK_SHADER_TYPE_PIXEL );
		l_cache.CreateTextureVariables( *l_program, p_textureFlags );
		auto & l_billboardUbo = l_program->CreateFrameVariableBuffer( ShaderProgram::BufferBillboards, MASK_SHADER_TYPE_GEOMETRY );
		l_billboardUbo.CreateVariable< Point2iFrameVariable >( ShaderProgram::Dimensions );

		ShaderObjectSPtr l_object = l_program->CreateObject( ShaderType::Geometry );
		l_object->SetInputType( l_input );
		l_object->SetOutputType( l_output );
		l_object->SetOutputVtxCount( l_count );

		String l_strVtxShader;
		{
			using namespace GLSL;
			auto l_writer = CreateGlslWriter();

			// Shader inputs
			Vec4 position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );

			// Shader outputs
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = vec4( position.xyz(), 1.0 );
			} );

			l_strVtxShader = l_writer.Finalise();
		}

		String l_strGeoShader;
		{
			using namespace GLSL;
			auto l_writer = CreateGlslWriter();

			l_writer.InputGeometryLayout( GetTopologyName( l_input ) );
			l_writer.OutputGeometryLayout( GetTopologyName( l_output ), l_count );

			// Shader inputs
			UBO_MATRIX( l_writer );
			UBO_SCENE( l_writer );
			UBO_BILLBOARD( l_writer );
			auto gl_in = l_writer.GetBuiltin< gl_PerVertex >( cuT( "gl_in" ), 8u );

			// Shader outputs
			auto vtx_worldSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
			auto vtx_worldViewSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldViewSpacePosition" ) );
			auto vtx_normal = l_writer.GetOutput< Vec3 >( cuT( "vtx_normal" ) );
			auto vtx_tangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_tangent" ) );
			auto vtx_bitangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_bitangent" ) );
			auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto l_pos = l_writer.GetLocale( cuT( "l_pos" ), gl_in[0].gl_Position().xyz() );
				auto l_toCamera = l_writer.GetLocale( cuT( "l_toCamera" ), normalize( vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) - l_pos ) );
				auto l_up = l_writer.GetLocale( cuT( "l_up" ), vec3( Float( 0 ), 1.0, 0.0 ) );
				auto l_left = l_writer.GetLocale( cuT( "l_left" ), cross( l_toCamera, l_up ) );

				auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vec3( l_toCamera.x(), 0.0, l_toCamera.z() ) ) );
				auto l_v3Tangent = l_writer.GetLocale( cuT( "l_v3Tangent" ), l_up );
				auto l_v3Bitangent = l_writer.GetLocale( cuT( "l_v3Bitangent" ), l_left );

				l_left *= c3d_v2iDimensions.x();
				l_up *= c3d_v2iDimensions.y();
				l_writer << Endl();
				{
					l_pos -= ( l_left * Float( 0.5 ) );
					vtx_worldSpacePosition = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).xyz();
					vtx_worldViewSpacePosition = l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz();
					gl_Position = c3d_mtxProjection * vec4( vtx_worldViewSpacePosition, 1.0 );
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = vec3( Float( 1.0 ), 0.0, 0.0 );
					l_writer.EmitVertex();
				}
				l_writer << Endl();
				{
					l_pos += l_up;
					vtx_worldSpacePosition = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).xyz();
					vtx_worldViewSpacePosition = l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz();
					gl_Position = c3d_mtxProjection * vec4( vtx_worldViewSpacePosition, 1.0 );
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
					vtx_worldSpacePosition = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).xyz();
					vtx_worldViewSpacePosition = l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz();
					gl_Position = c3d_mtxProjection * vec4( vtx_worldViewSpacePosition, 1.0 );
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = vec3( Float( 0.0 ), 0.0, 0.0 );
					l_writer.EmitVertex();
				}
				l_writer << Endl();
				{
					l_pos += l_up;
					vtx_worldSpacePosition = l_writer.Paren( c3d_mtxModel * vec4( l_pos, 1.0 ) ).xyz();
					vtx_worldViewSpacePosition = l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz();
					gl_Position = c3d_mtxProjection * vec4( vtx_worldViewSpacePosition, 1.0 );
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

		String l_strPxlShader = p_renderPass.GetPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
		auto l_model = GetGpuInformations().GetMaxShaderModel();
		l_program->SetSource( ShaderType::Vertex, l_model, l_strVtxShader );
		l_program->SetSource( ShaderType::Geometry, l_model, l_strGeoShader );
		l_program->SetSource( ShaderType::Pixel, l_model, l_strPxlShader );

		return l_program;
	}

	void RenderSystem::SetCurrentContext( Context * p_context )
	{
		m_currentContexts[std::this_thread::get_id()] = p_context;
	}

	Context * RenderSystem::GetCurrentContext()
	{
		Context * l_return{ nullptr };
		auto l_it = m_currentContexts.find( std::this_thread::get_id() );

		if ( l_it != m_currentContexts.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
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
