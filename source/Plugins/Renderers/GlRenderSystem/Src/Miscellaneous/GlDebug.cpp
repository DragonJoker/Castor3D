#include "Miscellaneous/GlDebug.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#	define GL_INVALID_FRAMEBUFFER_OPERATION 0x506
#endif

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDebug::GlDebug( GlRenderSystem & p_renderSystem )
		: Holder{ p_renderSystem.GetOpenGl() }
		, m_renderSystem{ p_renderSystem }
	{
		m_pfnGetError = &glGetError;
	}

	GlDebug::~GlDebug()
	{
	}

	void GlDebug::Initialise()
	{
		auto & l_gl = GetOpenGl();

		if ( l_gl.HasExtension( KHR_debug ) )
		{
			gl_api::GetFunction( m_pfnDebugMessageCallback, cuT( "glDebugMessageCallback" ), cuT( "KHR" ) );
		}
		else if ( l_gl.HasExtension( ARB_debug_output ) )
		{
			gl_api::GetFunction( m_pfnDebugMessageCallback, cuT( "glDebugMessageCallback" ), cuT( "ARB" ) );
		}
		else if ( l_gl.HasExtension( AMDX_debug_output ) )
		{
			gl_api::GetFunction( m_pfnDebugMessageCallbackAMD, cuT( "glDebugMessageCallbackAMD" ), cuT( "" ) );
		}

		if ( m_pfnDebugMessageCallback )
		{
			m_pfnDebugMessageCallback( PFNGLDEBUGPROC( &GlDebug::StDebugLog ), this );
		}

		if ( m_pfnDebugMessageCallbackAMD )
		{
			m_pfnDebugMessageCallbackAMD( PFNGLDEBUGAMDPROC( &GlDebug::StDebugLogAMD ), this );
		}
		
		if ( m_pfnDebugMessageCallback || m_pfnDebugMessageCallbackAMD )
		{
			GetOpenGl().Enable( GlTweak::eDebugOutputSynchronous );
		}
	}

	void GlDebug::Cleanup()
	{
		m_pfnGetError = nullptr;
		m_pfnDebugMessageCallback = nullptr;
		m_pfnDebugMessageCallbackAMD = nullptr;
	}

	bool GlDebug::GlCheckError( std::string const & p_text )const
	{
		return DoGlCheckError( string::string_cast< xchar >( p_text ) );
	}

	bool GlDebug::GlCheckError( std::wstring const & p_text )const
	{
		return DoGlCheckError( string::string_cast< xchar >( p_text ) );
	}

#if !defined( NDEBUG )

	bool GlDebug::Track( void * p_object
		, std::string const & p_name
		, std::string const & p_file
		, int p_line )const
	{
		return m_renderSystem.Track( p_object, p_name, p_file, p_line );
	}

	bool GlDebug::UnTrack( void * p_object )const
	{
		return m_renderSystem.Untrack( p_object );
	}

#endif

	void GlDebug::StDebugLog( GlDebugSource source
		, GlDebugType type
		, uint32_t id
		, GlDebugSeverity severity
		, int length
		, const char * message
		, void * userParam )
	{
		reinterpret_cast< GlDebug * >( userParam )->DebugLog( source, type, id, severity, length, message );
	}

	void GlDebug::StDebugLogAMD( uint32_t id
		, GlDebugCategory category
		, GlDebugSeverity severity
		, int length
		, const char * message
		, void * userParam )
	{
		reinterpret_cast< GlDebug * >( userParam )->DebugLogAMD( id, category, severity, length, message );
	}

	bool GlDebug::DoGlCheckError( String const & p_text )const
	{
		static std::map< uint32_t, String > const Errors
		{
			{ GL_INVALID_ENUM, cuT( "Invalid Enum" ) },
			{ GL_INVALID_VALUE, cuT( "Invalid Value" ) },
			{ GL_INVALID_OPERATION, cuT( "Invalid Operation" ) },
			{ GL_STACK_OVERFLOW, cuT( "Stack Overflow" ) },
			{ GL_STACK_UNDERFLOW, cuT( "Stack Underflow" ) },
			{ GL_OUT_OF_MEMORY, cuT( "Out of memory" ) },
			{ GL_INVALID_FRAMEBUFFER_OPERATION, cuT( "Invalid frame buffer operation" ) },
		};

		bool l_return = true;
		uint32_t l_errorCode = m_pfnGetError();

		if ( l_errorCode )
		{
			//auto l_it = Errors.find( l_errorCode );
			//StringStream l_error;
			//l_error << cuT( "OpenGL Error, on function: " ) << p_text << std::endl;
			//l_error << cuT( "  ID: 0x" ) << std::hex << l_errorCode << std::endl;

			//if ( l_it == Errors.end() )
			//{
			//	l_error << cuT( "  Message: Unknown error" ) << std::endl;
			//}
			//else
			//{
			//	l_error << cuT( "  Message: " ) << l_it->second << std::endl;
			//}

			//String l_sysError = System::GetLastErrorText();

			//if ( !l_sysError.empty() )
			//{
			//	l_error << cuT( "  System: " ) << l_sysError << std::endl;
			//}

			//l_error << Debug::Backtrace{ 20, 4 };
			//Logger::LogError( l_error );
			l_return = false;
			l_errorCode = m_pfnGetError();
		}

		return l_return;
	}

	void GlDebug::DebugLog( GlDebugSource source
		, GlDebugType type
		, uint32_t id
		, GlDebugSeverity severity
		, int CU_PARAM_UNUSED( length )
		, const char * message )const
	{
		if ( id != 131185
			 && id != 131186
			 && id != 131154 )
		{
			bool l_error = false;
			StringStream l_toLog;
			l_toLog << cuT( "OpenGl Debug\n  Source: " );

			switch ( source )
			{
			case GlDebugSource::eAPI:
				l_toLog << cuT( "OpenGL" );
				break;

			case GlDebugSource::eWindowSystem:
				l_toLog << cuT( "Window System" );
				break;

			case GlDebugSource::eShaderCompiler:
				l_toLog << cuT( "Shader compiler" );
				break;

			case GlDebugSource::eThirdParty:
				l_toLog << cuT( "Third party" );
				break;

			case GlDebugSource::eApplication:
				l_toLog << cuT( "Application" );
				break;

			case GlDebugSource::eOther:
				l_toLog << cuT( "Other" );
				break;

			default:
				l_toLog << cuT( "Undefined" );
				break;
			}

			l_toLog << cuT( "\n  Type: " );

			switch ( type )
			{
			case GlDebugType::eError:
				l_toLog << cuT( "Error" );
				break;

			case GlDebugType::eDeprecatedBehavior:
				l_toLog << cuT( "Deprecated behavior" );
				break;

			case GlDebugType::eUndefinedBehavior:
				l_toLog << cuT( "Undefined behavior" );
				break;

			case GlDebugType::ePortability:
				l_toLog << cuT( "Portability" );
				break;

			case GlDebugType::ePerformance:
				l_toLog << cuT( "Performance" );
				break;

			case GlDebugType::eOther:
				l_toLog << cuT( "Other" );
				break;

			case GlDebugType::eMarker:
				l_toLog << cuT( "Marker" );
				break;

			case GlDebugType::ePushGroup:
				l_toLog << cuT( "Push Group" );
				break;

			case GlDebugType::ePopGroup:
				l_toLog << cuT( "Pop Group" );
				break;

			default:
				l_toLog << cuT( "Undefined" );
				break;
			}

			l_toLog << cuT( "\n  ID: " ) + string::to_string( id ) + cuT( "\n  Severity: " );

			switch ( severity )
			{
			case GlDebugSeverity::eHigh:
				l_error = true;
				l_toLog << cuT( "High" );
				break;

			case GlDebugSeverity::eMedium:
				l_toLog << cuT( "Medium" );
				break;

			case GlDebugSeverity::eLow:
				l_toLog << cuT( "Low" );
				break;

			case GlDebugSeverity::eNotification:
				l_toLog << cuT( "Notification" );
				break;

			default:
				l_toLog << cuT( "Undefined" );
				break;
			}

			l_toLog << cuT( "\n  Message: " ) << string::string_cast< xchar >( message );

			if ( l_error )
			{
				l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
				Logger::LogError( l_toLog );
			}
			else
			{
				Logger::LogWarning( l_toLog );
			}
		}
	}

	void GlDebug::DebugLogAMD( uint32_t id
		, GlDebugCategory category
		, GlDebugSeverity severity
		, int CU_PARAM_UNUSED( length )
		, const char * message )const
	{
		bool l_error = false;
		StringStream l_toLog;
		l_toLog << cuT( "OpenGl Debug\n  Category: " );

		switch ( category )
		{
		case GlDebugCategory::eAPIError:
			l_toLog << cuT( "OpenGL" );
			break;

		case GlDebugCategory::eWindowSystem:
			l_toLog << cuT( "Windows" );
			break;

		case GlDebugCategory::eDeprecation:
			l_toLog << cuT( "Deprecated behavior" );
			break;

		case GlDebugCategory::eUndefinedBehavior:
			l_toLog << cuT( "Undefined behavior" );
			break;

		case GlDebugCategory::ePerformance:
			l_toLog << cuT( "Performance" );
			break;

		case GlDebugCategory::eShaderCompiler:
			l_toLog << cuT( "Shader compiler" );
			break;

		case GlDebugCategory::eApplication:
			l_toLog << cuT( "Application" );
			break;

		case GlDebugCategory::eOther:
			l_toLog << cuT( "Other" );
			break;

		default:
			l_toLog << cuT( "Undefined" );
			break;
		}

		l_toLog << cuT( "\n  ID: " ) << string::to_string( id ) << cuT( "\n  Severity: " );

		switch ( severity )
		{
		case GlDebugSeverity::eHigh:
			l_error = true;
			l_toLog << cuT( "High" );
			break;

		case GlDebugSeverity::eMedium:
			l_toLog << cuT( "Medium" );
			break;

		case GlDebugSeverity::eLow:
			l_toLog << cuT( "Low" );
			break;

		default:
			l_toLog << cuT( "Undefined" );
			break;
		}

		l_toLog << cuT( "\n  Message: " ) << string::string_cast< xchar >( message );

		if ( l_error )
		{
			l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 25, 2 };
			Logger::LogError( l_toLog );
		}
		else
		{
			Logger::LogWarning( l_toLog );
		}
	}
}
