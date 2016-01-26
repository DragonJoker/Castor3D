#include "GlShaderProgram.hpp"

#include "GlShaderObject.hpp"
#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "GlOneFrameVariable.hpp"
#include "GlFrameVariableBuffer.hpp"
#include "OpenGl.hpp"

#include "GlslSource.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlShaderProgram::GlShaderProgram( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: ShaderProgramBase( p_renderSystem, eSHADER_LANGUAGE_GLSL )
		, Object( p_gl,
				  "GlShaderProgram",
				  std::bind( &OpenGl::CreateProgram, std::ref( p_gl ) ),
				  std::bind( &OpenGl::DeleteProgram, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsProgram, std::ref( p_gl ), std::placeholders::_1 )
				  )
	{
		CreateObject( eSHADER_TYPE_VERTEX );
		CreateObject( eSHADER_TYPE_PIXEL );
	}

	GlShaderProgram::~GlShaderProgram()
	{
	}

	void GlShaderProgram::Cleanup()
	{
		DoCleanup();
		ObjectType::Destroy();
	}

	bool GlShaderProgram::Initialise()
	{
		bool l_return = true;

		if ( m_status != ePROGRAM_STATUS_LINKED )
		{
			ObjectType::Create();
			l_return = DoInitialise();
		}

		return l_return;
	}

	bool GlShaderProgram::Link()
	{
		bool l_return = false;
		int l_iLinked = 0;
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << uint32_t( m_activeShaders.size() ) );
		l_return &= GetOpenGl().LinkProgram( GetGlName() );
		l_return &= GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_LINK, &l_iLinked );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << l_iLinked );
		m_linkerLog = DoRetrieveLinkerLog();

		if ( l_iLinked && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogWarning( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			}

			l_return = DoLink();
		}
		else
		{
			Logger::LogError( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			m_status = ePROGRAM_STATUS_ERROR;
		}

		return l_return;
	}

	void GlShaderProgram::Bind( bool p_bindUbo )
	{
		if ( GetGlName() != eGL_INVALID_INDEX && m_status == ePROGRAM_STATUS_LINKED )
		{
			GetOpenGl().UseProgram( GetGlName() );
			DoBind( p_bindUbo );
		}
	}

	void GlShaderProgram::Unbind()
	{
		if ( GetGlName() != eGL_INVALID_INDEX && m_status == ePROGRAM_STATUS_LINKED )
		{
			uint32_t l_index = 0;

			for ( auto l_variableBuffer : m_listFrameVariableBuffers )
			{
				l_variableBuffer->Unbind( l_index++ );
			}

			DoUnbind();
			GetOpenGl().UseProgram( 0 );
		}
	}

	int GlShaderProgram::GetAttributeLocation( String const & p_name )const
	{
		int l_iReturn = eGL_INVALID_INDEX;

		if ( GetGlName() != eGL_INVALID_INDEX && GetOpenGl().IsProgram( GetGlName() ) )
		{
			l_iReturn = GetOpenGl().GetAttribLocation( GetGlName(), string::string_cast< char >( p_name ).c_str() );
		}

		return l_iReturn;
	}

	ShaderObjectBaseSPtr GlShaderProgram::DoCreateObject( eSHADER_TYPE p_type )
	{
		ShaderObjectBaseSPtr l_return = std::make_shared< GlShaderObject >( GetOpenGl(), this, p_type );
		return l_return;
	}

	std::shared_ptr< OneTextureFrameVariable > GlShaderProgram::DoCreateTextureVariable( int p_occurences )
	{
		return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( GetOpenGl(), p_occurences, this );
	}

	String GlShaderProgram::DoGetVertexShaderSource( uint32_t p_programFlags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( GetOwner() )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();
		// Vertex inputs
		ATTRIBUTE( l_writer, Vec4, vertex );
		ATTRIBUTE( l_writer, Vec3, normal );
		ATTRIBUTE( l_writer, Vec3, tangent );
		ATTRIBUTE( l_writer, Vec3, bitangent );
		ATTRIBUTE( l_writer, Vec3, texture );
		IVec4 bone_ids;
		Vec4 weights;
		Mat4 transform;

		if ( ( p_programFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
		{
			bone_ids = l_writer.GetAttribute< IVec4 >( cuT( "bone_ids" ) );
			weights = l_writer.GetAttribute< Vec4 >( cuT( "weights" ) );
		}

		if ( ( p_programFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			transform = l_writer.GetAttribute< Mat4 >( cuT( "transform" ) );
		}

		UBO_MATRIX( l_writer );

		// Outputs
		OUT( l_writer, Vec3, vtx_vertex );
		OUT( l_writer, Vec3, vtx_normal );
		OUT( l_writer, Vec3, vtx_tangent );
		OUT( l_writer, Vec3, vtx_bitangent );
		OUT( l_writer, Vec3, vtx_texture );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Vertex, vec4( vertex.XYZ, 1.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normal, vec4( normal, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangent, vec4( tangent, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Bitangent, vec4( bitangent, 0.0 ) );
			LOCALE( l_writer, Mat4, l_mtxModel );
			bool l_set = false;

			if ( ( p_programFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
			{
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxBoneTransform, c3d_mtxBones[bone_ids[Int( 0 )]] * weights[Int( 0 )] );
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 1 )]] * weights[Int( 1 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 2 )]] * weights[Int( 2 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 3 )]] * weights[Int( 3 )];
				//l_mtxBoneTransform = transpose( l_mtxBoneTransform );
				l_mtxModel = l_mtxBoneTransform;
				l_set = true;
			}

			if ( ( p_programFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
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

			vtx_texture = texture;
			vtx_vertex = l_writer.Paren( l_mtxModel * l_v4Vertex ).XYZ;
			vtx_normal = normalize( l_writer.Paren( l_mtxModel * l_v4Normal ).XYZ );
			vtx_tangent = normalize( l_writer.Paren( l_mtxModel * l_v4Tangent ).XYZ );
			vtx_bitangent = normalize( l_writer.Paren( l_mtxModel * l_v4Bitangent ).XYZ );
			BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * c3d_mtxView * l_mtxModel * l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String GlShaderProgram::DoRetrieveLinkerLog()
	{
		String l_log;

		if ( GetGlName() == eGL_INVALID_INDEX )
		{
			l_log = GetOpenGl().GetGlslErrorString( 2 );
		}
		else
		{
			int l_length = 0;
			GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_INFO_LOG_LENGTH, &l_length );

			if ( l_length > 1 )
			{
				std::vector< char > l_buffer( l_length );
				int l_written = 0;
				GetOpenGl().GetProgramInfoLog( GetGlName(), l_length, &l_written, l_buffer.data() );

				if ( l_written > 0 )
				{
					l_log = string::string_cast< xchar >( l_buffer.data(), l_buffer.data() + l_written );
				}
			}
		}

		return l_log;
	}
}
