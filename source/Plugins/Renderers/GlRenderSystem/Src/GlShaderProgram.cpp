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
		: ShaderProgram( p_renderSystem, eSHADER_LANGUAGE_GLSL )
		, Object( p_gl,
				  "GlShaderProgram",
				  std::bind( &OpenGl::CreateProgram, std::ref( p_gl ) ),
				  std::bind( &OpenGl::DeleteProgram, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsProgram, std::ref( p_gl ), std::placeholders::_1 )
				)
		, m_layout( p_gl, p_renderSystem )
	{
		CreateObject( eSHADER_TYPE_VERTEX );
		CreateObject( eSHADER_TYPE_PIXEL );
	}

	GlShaderProgram::~GlShaderProgram()
	{
	}

	void GlShaderProgram::Cleanup()
	{
		m_layout.Cleanup();
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

			if ( l_return )
			{
				m_layout.Initialise( *this );
			}
		}

		return l_return;
	}

	bool GlShaderProgram::Link()
	{
		bool l_return = false;
		ENSURE( GetGlName() != eGL_INVALID_INDEX );
		int l_attached = 0;
		l_return &= GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_ATTACHED_SHADERS, &l_attached );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << l_attached );
		l_return &= GetOpenGl().LinkProgram( GetGlName() );
		int l_linked = 0;
		l_return &= GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_LINK, &l_linked );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << l_linked );
		m_linkerLog = DoRetrieveLinkerLog();

		if ( l_linked && l_attached == int( m_activeShaders.size() ) && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogWarning( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			}

			l_return = DoLink();
		}
		else
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogError( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			}

			if ( l_attached != int( m_activeShaders.size() ) )
			{
				Logger::LogError( cuT( "GlShaderProgram::Link - The linked shaders count doesn't match the active shaders count." ) );
			}

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

	ShaderObjectSPtr GlShaderProgram::DoCreateObject( eSHADER_TYPE p_type )
	{
		ShaderObjectSPtr l_return = std::make_shared< GlShaderObject >( GetOpenGl(), this, p_type );
		return l_return;
	}

	std::shared_ptr< OneIntFrameVariable > GlShaderProgram::DoCreateTextureVariable( int p_occurences )
	{
		return std::make_shared< GlOneFrameVariable< int > >( GetOpenGl(), p_occurences, this );
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
