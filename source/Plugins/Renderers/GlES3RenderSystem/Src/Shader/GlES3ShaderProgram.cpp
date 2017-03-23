#include "Shader/GlES3ShaderProgram.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"
#include "Shader/GlES3PushUniform.hpp"
#include "Shader/GlES3ShaderObject.hpp"

#include <GlslSource.hpp>

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3ShaderProgram::GlES3ShaderProgram( OpenGlES3 & p_gl, GlES3RenderSystem & p_renderSystem )
		: ShaderProgram( p_renderSystem )
		, Object( p_gl,
				  "GlES3ShaderProgram",
				  std::bind( &OpenGlES3::CreateProgram, std::ref( p_gl ) ),
				  std::bind( &OpenGlES3::DeleteProgram, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGlES3::IsProgram, std::ref( p_gl ), std::placeholders::_1 )
				)
		, m_layout( p_gl, p_renderSystem )
	{
	}

	GlES3ShaderProgram::~GlES3ShaderProgram()
	{
	}

	void GlES3ShaderProgram::Cleanup()
	{
		m_layout.Cleanup();
		DoCleanup();
		ObjectType::Destroy();
	}

	bool GlES3ShaderProgram::Initialise()
	{
		bool l_return = true;

		if ( m_status != ProgramStatus::eLinked )
		{
			ObjectType::Create();
			l_return = DoInitialise();

			if ( l_return )
			{
				m_layout.Initialise( *this );
			}
		}

		return l_return;
	}

	bool GlES3ShaderProgram::Link()
	{
		DoBindTransformLayout();
		ENSURE( GetGlES3Name() != GlES3InvalidIndex );
		int l_attached = 0;
		GetOpenGlES3().GetProgramiv( GetGlES3Name(), GlES3ShaderStatus::eAttachedShaders, &l_attached );
		Logger::LogDebug( StringStream() << cuT( "GlES3ShaderProgram::Link - Programs attached : " ) << l_attached );
		GetOpenGlES3().LinkProgram( GetGlES3Name() );
		int l_linked = 0;
		GetOpenGlES3().GetProgramiv( GetGlES3Name(), GlES3ShaderStatus::eLink, &l_linked );
		Logger::LogDebug( StringStream() << cuT( "GlES3ShaderProgram::Link - Program link status : " ) << l_linked );
		m_linkerLog = DoRetrieveLinkerLog();
		bool l_return = false;

		if ( l_linked && l_attached == int( m_activeShaders.size() ) && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogWarning( cuT( "GlES3ShaderProgram::Link - " ) + m_linkerLog );
			}

			l_return = DoLink();
		}
		else
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogError( cuT( "GlES3ShaderProgram::Link - " ) + m_linkerLog );
			}

			if ( l_attached != int( m_activeShaders.size() ) )
			{
				Logger::LogError( cuT( "GlES3ShaderProgram::Link - The linked shaders count doesn't match the active shaders count." ) );
			}

			m_status = ProgramStatus::eError;
		}

		return l_return;
	}

	void GlES3ShaderProgram::Bind()const
	{
		REQUIRE( GetGlES3Name() != GlES3InvalidIndex && m_status == ProgramStatus::eLinked );
		GetOpenGlES3().UseProgram( GetGlES3Name() );
		DoBind();
	}

	void GlES3ShaderProgram::Unbind()const
	{
		REQUIRE( GetGlES3Name() != GlES3InvalidIndex && m_status == ProgramStatus::eLinked );
		DoUnbind();
		GetOpenGlES3().UseProgram( 0 );
	}

	int GlES3ShaderProgram::GetAttributeLocation( String const & p_name )const
	{
		int l_iReturn = int( GlES3InvalidIndex );

		if ( GetGlES3Name() != GlES3InvalidIndex && GetOpenGlES3().IsProgram( GetGlES3Name() ) )
		{
			l_iReturn = GetOpenGlES3().GetAttribLocation( GetGlES3Name(), string::string_cast< char >( p_name ).c_str() );
		}

		return l_iReturn;
	}

	ShaderObjectSPtr GlES3ShaderProgram::DoCreateObject( ShaderType p_type )
	{
		ShaderObjectSPtr l_return = std::make_shared< GlES3ShaderObject >( GetOpenGlES3(), this, p_type );
		return l_return;
	}

	std::shared_ptr< PushUniform > GlES3ShaderProgram::DoCreateUniform( UniformType p_type, int p_occurences )
	{
		switch ( p_type )
		{
		case UniformType::eBool:
			return std::make_shared< GlES3PushUniform< UniformType::eBool > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eInt:
			return std::make_shared< GlES3PushUniform< UniformType::eInt > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eUInt:
			return std::make_shared< GlES3PushUniform< UniformType::eUInt > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eFloat:
			return std::make_shared< GlES3PushUniform< UniformType::eFloat > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eDouble:
			return std::make_shared< GlES3PushUniform< UniformType::eDouble > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eSampler:
			return std::make_shared< GlES3PushUniform< UniformType::eSampler > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec2b:
			return std::make_shared< GlES3PushUniform< UniformType::eVec2b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec3b:
			return std::make_shared< GlES3PushUniform< UniformType::eVec3b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec4b:
			return std::make_shared< GlES3PushUniform< UniformType::eVec4b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec2i:
			return std::make_shared< GlES3PushUniform< UniformType::eVec2i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec3i:
			return std::make_shared< GlES3PushUniform< UniformType::eVec3i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec4i:
			return std::make_shared< GlES3PushUniform< UniformType::eVec4i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec2ui:
			return std::make_shared< GlES3PushUniform< UniformType::eVec2ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec3ui:
			return std::make_shared< GlES3PushUniform< UniformType::eVec3ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec4ui:
			return std::make_shared< GlES3PushUniform< UniformType::eVec4ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec2f:
			return std::make_shared< GlES3PushUniform< UniformType::eVec2f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec3f:
			return std::make_shared< GlES3PushUniform< UniformType::eVec3f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec4f:
			return std::make_shared< GlES3PushUniform< UniformType::eVec4f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec2d:
			return std::make_shared< GlES3PushUniform< UniformType::eVec2d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec3d:
			return std::make_shared< GlES3PushUniform< UniformType::eVec3d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eVec4d:
			return std::make_shared< GlES3PushUniform< UniformType::eVec4d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x2b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x2b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x3b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x3b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x4b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x4b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x2b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x2b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x3b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x3b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x4b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x4b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x2b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x2b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x3b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x3b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x4b:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x4b > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x2i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x2i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x3i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x3i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x4i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x4i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x2i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x2i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x3i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x3i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x4i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x4i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x2i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x2i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x3i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x3i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x4i:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x4i > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x2ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x2ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x3ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x3ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x4ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x4ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x2ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x2ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x3ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x3ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x4ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x4ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x2ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x2ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x3ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x3ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x4ui:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x4ui > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x2f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x2f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x3f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x3f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x4f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x4f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x2f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x2f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x3f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x3f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x4f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x4f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x2f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x2f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x3f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x3f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x4f:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x4f > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x2d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x2d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x3d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x3d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat2x4d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat2x4d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x2d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x2d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x3d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x3d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat3x4d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat3x4d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x2d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x2d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x3d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x3d > >( GetOpenGlES3(), *this, p_occurences );

		case UniformType::eMat4x4d:
			return std::make_shared< GlES3PushUniform< UniformType::eMat4x4d > >( GetOpenGlES3(), *this, p_occurences );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}

	String GlES3ShaderProgram::DoRetrieveLinkerLog()
	{
		String l_log;

		if ( GetGlES3Name() == GlES3InvalidIndex )
		{
			l_log = GetOpenGlES3().GetGlslErrorString( 2 );
		}
		else
		{
			int l_length = 0;
			GetOpenGlES3().GetProgramiv( GetGlES3Name(), GlES3ShaderStatus::eInfoLogLength, &l_length );

			if ( l_length > 1 )
			{
				std::vector< char > l_buffer( l_length );
				int l_written = 0;
				GetOpenGlES3().GetProgramInfoLog( GetGlES3Name(), l_length, &l_written, l_buffer.data() );

				if ( l_written > 0 )
				{
					l_log = string::string_cast< xchar >( l_buffer.data(), l_buffer.data() + l_written );
				}
			}
		}

		return l_log;
	}

	void GlES3ShaderProgram::DoBindTransformLayout()
	{
		if ( m_declaration.size() > 0 )
		{
			std::vector< char const * > l_varyings;
			l_varyings.reserve( m_declaration.size() );

			for ( auto & l_element : m_declaration )
			{
				l_varyings.push_back( l_element.m_name.c_str() );
			}

			GetOpenGlES3().TransformFeedbackVaryings( GetGlES3Name(), int( l_varyings.size() ), l_varyings.data(), GlES3AttributeLayout::eInterleaved );
		}
	}
}
