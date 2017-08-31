#include "Miscellaneous/GlDebug.hpp"

#include "Common/OpenGl.hpp"
#include "Common/GlGetFunction.hpp"
#include "Render/GlRenderSystem.hpp"

#include <map>
#include <iomanip>

#include <GL/gl.h>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlDebug::GlDebug( GlRenderSystem & renderSystem )
		: Holder{ renderSystem.getOpenGl() }
		, m_renderSystem{ renderSystem }
	{
		m_pfnGetError = &glGetError;
	}

	GlDebug::~GlDebug()
	{
	}

	void GlDebug::initialise()
	{
		auto & gl = getOpenGl();

		if ( gl.hasExtension( KHR_debug ) )
		{
			gl_api::getFunction( m_pfnDebugMessageCallback, cuT( "glDebugMessageCallback" ), cuT( "KHR" ) );
		}
		else if ( gl.hasExtension( ARB_debug_output ) )
		{
			gl_api::getFunction( m_pfnDebugMessageCallback, cuT( "glDebugMessageCallback" ), cuT( "ARB" ) );
		}
		else if ( gl.hasExtension( AMDX_debug_output ) )
		{
			gl_api::getFunction( m_pfnDebugMessageCallbackAMD, cuT( "glDebugMessageCallbackAMD" ), cuT( "" ) );
		}

		if ( m_pfnDebugMessageCallback )
		{
			m_pfnDebugMessageCallback( PFNGLDEBUGPROC( &GlDebug::debugLog ), this );
		}

		if ( m_pfnDebugMessageCallbackAMD )
		{
			m_pfnDebugMessageCallbackAMD( PFNGLDEBUGAMDPROC( &GlDebug::debugLogAMD ), this );
		}
		
		if ( m_pfnDebugMessageCallback || m_pfnDebugMessageCallbackAMD )
		{
			getOpenGl().Enable( GlTweak::eDebugOutputSynchronous );
		}

		filterMessage( 0x00020072 );
		// TODO: Investigate on why I get this noisy message
		filterMessage( 0x00020096 );
	}

	void GlDebug::cleanup()
	{
		m_pfnGetError = nullptr;
		m_pfnDebugMessageCallback = nullptr;
		m_pfnDebugMessageCallbackAMD = nullptr;
	}

	bool GlDebug::checkError( std::string const & p_text )const
	{
		return doGlCheckError( string::stringCast< xchar >( p_text ) );
	}

	bool GlDebug::checkError( std::wstring const & p_text )const
	{
		return doGlCheckError( string::stringCast< xchar >( p_text ) );
	}

	void GlDebug::filterMessage( uint32_t p_message )
	{
		m_filteredOut.insert( p_message );
	}

	bool GlDebug::isFiltered( uint32_t p_message )const
	{
		return m_filteredOut.find( p_message ) != m_filteredOut.end();
	}

	void GlDebug::BindTexture( uint32_t p_name, uint32_t p_index )const
	{
		auto it = m_textureUnits.find( p_index );

		if ( !p_name )
		{
			if ( it == m_textureUnits.end()
				|| it->second.m_texture.m_name == 0u )
			{
				Logger::logError( std::stringstream{} << "double texture unbind for index " << p_index );
			}
		}
		else if ( it != m_textureUnits.end()
			&& it->second.m_texture.m_name != 0u )
		{
			Logger::logError( std::stringstream{} << "Previous texture " << it->second.m_texture.m_name << " at index " << p_index << " has not been unbound " );
		}

		m_textureUnits[p_index].m_texture = Binding{ p_name };

		if ( !p_name )
		{
			doUpdateTextureUnits();
		}
	}

	void GlDebug::bindSampler( uint32_t p_name, uint32_t p_index )const
	{
		auto it = m_textureUnits.find( p_index );

		if ( !p_name )
		{
			if ( it == m_textureUnits.end()
				|| it->second.m_sampler.m_name == 0u )
			{
				Logger::logError( std::stringstream{} << "double sampler unbind for index " << p_index );
			}
		}
		else if ( it != m_textureUnits.end()
			&& it->second.m_sampler.m_name != 0u )
		{
			Logger::logError( std::stringstream{} << "Previous sampler " << it->second.m_sampler.m_name << " at index " << p_index << " has not been unbound " );
		}

		m_textureUnits[p_index].m_sampler = Binding{ p_name };

		if ( !p_name )
		{
			doUpdateTextureUnits();
		}
	}

	void GlDebug::checkTextureUnits()const
	{
		for ( auto & it : m_textureUnits )
		{
			if ( !it.second.m_sampler.m_name
				|| !it.second.m_texture.m_name )
			{
				Logger::logError( std::stringstream{} << "Texture unit at index " << it.first << " is incomplete" );
			}
		}
	}

	void GlDebug::doUpdateTextureUnits()const
	{
		auto it = m_textureUnits.begin();

		while ( it != m_textureUnits.end() )
		{
			if ( !it->second.m_sampler.m_name
				&& !it->second.m_texture.m_name )
			{
				it = m_textureUnits.erase( it );
			}
			else
			{
				++it;
			}
		}
	}

#if !defined( NDEBUG )

	bool GlDebug::track( void * p_object
		, std::string const & p_name
		, std::string const & p_file
		, int p_line )const
	{
		return m_renderSystem.track( p_object, p_name, p_file, p_line );
	}

	bool GlDebug::untrack( void * p_object )const
	{
		return m_renderSystem.untrack( p_object );
	}

#endif

	void GlDebug::debugLog( GlDebugSource source
		, GlDebugType type
		, uint32_t id
		, GlDebugSeverity severity
		, int length
		, const char * message
		, void * userParam )
	{
		reinterpret_cast< GlDebug * >( userParam )->doDebugLog( source, type, id, severity, length, message );
	}

	void GlDebug::debugLogAMD( uint32_t id
		, GlDebugCategory category
		, GlDebugSeverity severity
		, int length
		, const char * message
		, void * userParam )
	{
		reinterpret_cast< GlDebug * >( userParam )->doDebugLogAMD( id, category, severity, length, message );
	}

	bool GlDebug::doGlCheckError( String const & p_text )const
	{
		static uint32_t constexpr InvalidEnum = 0x0500;
		static uint32_t constexpr InvalidValue = 0x0501;
		static uint32_t constexpr InvalidOperation = 0x0502;
		static uint32_t constexpr StackOverflow = 0x0503;
		static uint32_t constexpr StackUnderflow = 0x0504;
		static uint32_t constexpr OutOfMemory = 0x0505;
		static uint32_t constexpr InvalidFramebufferOperation = 0x0506;

		static std::map< uint32_t, String > const Errors
		{
			{ InvalidEnum, cuT( "Invalid Enum" ) },
			{ InvalidValue, cuT( "Invalid Value" ) },
			{ InvalidOperation, cuT( "Invalid Operation" ) },
			{ StackOverflow, cuT( "Stack Overflow" ) },
			{ StackUnderflow, cuT( "Stack Underflow" ) },
			{ OutOfMemory, cuT( "Out of memory" ) },
			{ InvalidFramebufferOperation, cuT( "Invalid frame buffer operation" ) },
		};

		bool result = true;
		uint32_t errorCode = m_pfnGetError();

		if ( errorCode )
		{
			//auto it = Errors.find( errorCode );
			//StringStream error;
			//l_error << cuT( "OpenGL Error, on function: " ) << p_text << std::endl;
			//l_error << cuT( "  ID: 0x" ) << std::hex << errorCode << std::endl;

			//if ( it == Errors.end() )
			//{
			//	error << cuT( "  Message: Unknown error" ) << std::endl;
			//}
			//else
			//{
			//	error << cuT( "  Message: " ) << it->second << std::endl;
			//}

			//String sysError = System::getLastErrorText();

			//if ( !sysError.empty() )
			//{
			//	error << cuT( "  System: " ) << sysError << std::endl;
			//}

			//l_error << Debug::Backtrace{ 20, 4 };
			//Logger::logError( error );
			result = false;
			errorCode = m_pfnGetError();
		}

		return result;
	}

	void GlDebug::doDebugLog( GlDebugSource source
		, GlDebugType type
		, uint32_t id
		, GlDebugSeverity severity
		, int CU_PARAM_UNUSED( length )
		, const char * message )const
	{
		static std::map< GlDebugSource, String > SourceName
		{
			{ GlDebugSource::eAPI, cuT( "OpenGl" ) },
			{ GlDebugSource::eWindowSystem, cuT( "Window System" ) },
			{ GlDebugSource::eShaderCompiler, cuT( "Shader compiler" ) },
			{ GlDebugSource::eThirdParty, cuT( "Third party" ) },
			{ GlDebugSource::eApplication, cuT( "Application" ) },
			{ GlDebugSource::eOther, cuT( "Other" ) },
		};

		static std::map< GlDebugType, String > TypeName
		{
			{ GlDebugType::eError, cuT( "Error" ) },
			{ GlDebugType::eDeprecatedBehavior, cuT( "Deprecated behavior" ) },
			{ GlDebugType::eUndefinedBehavior, cuT( "Undefined behavior" ) },
			{ GlDebugType::ePortability, cuT( "Portability" ) },
			{ GlDebugType::ePerformance, cuT( "Performance" ) },
			{ GlDebugType::eOther, cuT( "Other" ) },
			{ GlDebugType::eMarker, cuT( "Marker" ) },
			{ GlDebugType::ePushGroup, cuT( "Push Group" ) },
			{ GlDebugType::ePopGroup, cuT( "Pop Group" ) },
		};

		static std::map< GlDebugSeverity, String > SeverityName
		{
			{ GlDebugSeverity::eHigh, cuT( "High" ) },
			{ GlDebugSeverity::eMedium, cuT( "Medium" ) },
			{ GlDebugSeverity::eLow, cuT( "Low" ) },
			{ GlDebugSeverity::eNotification, cuT( "Notification" ) },
		};

		if ( !isFiltered( id ) )
		{
			StringStream toLog;
			auto msg = string::stringCast< xchar >( message );
			string::replace( msg, '\n', ' ' );
			toLog << cuT( "OpenGl Debug\n " );
			toLog << cuT( "  Source: " ) << SourceName[source] << cuT( "\n" );
			toLog << cuT( "  Type: " ) << TypeName[type] << cuT( "\n" );
			toLog << cuT( "  Severity: " ) << SeverityName[severity] << cuT( "\n" );
			toLog << cuT( "  ID: 0x" ) << std::hex << std::setfill( '0' ) << std::setw( 8 ) << id << cuT( "\n" );
			toLog << cuT( "  Message: " ) << message;

			switch ( severity )
			{
			case GlDebugSeverity::eHigh:
				toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
				Logger::logError( toLog );
				break;

			case GlDebugSeverity::eMedium:
				Logger::logWarning( toLog );
				break;

			case GlDebugSeverity::eLow:
				Logger::logInfo( toLog );
				break;

			case GlDebugSeverity::eNotification:
				Logger::logTrace( toLog );
				break;

			default:
				Logger::logWarning( toLog );
				break;
			}
		}
	}

	void GlDebug::doDebugLogAMD( uint32_t id
		, GlDebugCategory category
		, GlDebugSeverity severity
		, int CU_PARAM_UNUSED( length )
		, const char * message )const
	{
		static std::map< GlDebugCategory, String > CategoryName
		{
			{ GlDebugCategory::eAPIError, cuT( "OpenGl" ) },
			{ GlDebugCategory::eWindowSystem, cuT( "Window System" ) },
			{ GlDebugCategory::eDeprecation, cuT( "Deprecated Behavior" ) },
			{ GlDebugCategory::eUndefinedBehavior, cuT( "Undefined Behavior" ) },
			{ GlDebugCategory::ePerformance, cuT( "Performance" ) },
			{ GlDebugCategory::eShaderCompiler, cuT( "Shader Compiler" ) },
			{ GlDebugCategory::eApplication, cuT( "Application" ) },
			{ GlDebugCategory::eOther, cuT( "Other" ) }
		};

		static std::map< GlDebugSeverity, String > SeverityName
		{
			{ GlDebugSeverity::eHigh, cuT( "High" ) },
			{ GlDebugSeverity::eMedium, cuT( "Medium" ) },
			{ GlDebugSeverity::eLow, cuT( "Low" ) },
			{ GlDebugSeverity::eNotification, cuT( "Notification" ) }
		};

		StringStream toLog;
		auto msg = string::stringCast< xchar >( message );
		string::replace( msg, '\n', ' ' );
		toLog << cuT( "OpenGl Debug\n" );
		toLog << cuT( "  Category: " ) << CategoryName[category] << cuT( "\n" );
		toLog << cuT( "  Severity: " ) << SeverityName[severity] << cuT( "\n" );
		toLog << cuT( "  ID: 0x" ) << std::hex << std::setfill( '0' ) << std::setw( 8 ) << id << cuT( "\n" );
		toLog << cuT( "  Message: " ) << message;

		switch ( severity )
		{
		case GlDebugSeverity::eHigh:
			toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
			Logger::logError( toLog );
			break;

		case GlDebugSeverity::eMedium:
			Logger::logWarning( toLog );
			break;

		case GlDebugSeverity::eLow:
			Logger::logInfo( toLog );
			break;

		case GlDebugSeverity::eNotification:
			Logger::logTrace( toLog );
			break;

		default:
			Logger::logWarning( toLog );
			break;
		}
	}
}
