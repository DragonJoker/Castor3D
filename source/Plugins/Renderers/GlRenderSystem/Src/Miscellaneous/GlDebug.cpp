#include "Miscellaneous/GlDebug.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <map>
#include <iomanip>

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
		auto & gl = GetOpenGl();

		if ( gl.HasExtension( KHR_debug ) )
		{
			gl_api::GetFunction( m_pfnDebugMessageCallback, cuT( "glDebugMessageCallback" ), cuT( "KHR" ) );
		}
		else if ( gl.HasExtension( ARB_debug_output ) )
		{
			gl_api::GetFunction( m_pfnDebugMessageCallback, cuT( "glDebugMessageCallback" ), cuT( "ARB" ) );
		}
		else if ( gl.HasExtension( AMDX_debug_output ) )
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

		FilterMessage( 0x00020072 );
		// TODO: Investigate on why I get this noisy message
		FilterMessage( 0x00020096 );
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

	void GlDebug::FilterMessage( uint32_t p_message )
	{
		m_filteredOut.insert( p_message );
	}

	bool GlDebug::IsFiltered( uint32_t p_message )const
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
				Logger::LogError( std::stringstream{} << "Double texture unbind for index " << p_index );
			}
		}
		else if ( it != m_textureUnits.end()
			&& it->second.m_texture.m_name != 0u )
		{
			Logger::LogError( std::stringstream{} << "Previous texture " << it->second.m_texture.m_name << " at index " << p_index << " has not been unbound " );
		}

		m_textureUnits[p_index].m_texture = Binding{ p_name };

		if ( !p_name )
		{
			DoUpdateTextureUnits();
		}
	}

	void GlDebug::BindSampler( uint32_t p_name, uint32_t p_index )const
	{
		auto it = m_textureUnits.find( p_index );

		if ( !p_name )
		{
			if ( it == m_textureUnits.end()
				|| it->second.m_sampler.m_name == 0u )
			{
				Logger::LogError( std::stringstream{} << "Double sampler unbind for index " << p_index );
			}
		}
		else if ( it != m_textureUnits.end()
			&& it->second.m_sampler.m_name != 0u )
		{
			Logger::LogError( std::stringstream{} << "Previous sampler " << it->second.m_sampler.m_name << " at index " << p_index << " has not been unbound " );
		}

		m_textureUnits[p_index].m_sampler = Binding{ p_name };

		if ( !p_name )
		{
			DoUpdateTextureUnits();
		}
	}

	void GlDebug::CheckTextureUnits()const
	{
		for ( auto & it : m_textureUnits )
		{
			if ( !it.second.m_sampler.m_name
				|| !it.second.m_texture.m_name )
			{
				Logger::LogError( std::stringstream{} << "Texture unit at index " << it.first << " is incomplete" );
			}
		}
	}

	void GlDebug::DoUpdateTextureUnits()const
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

			//String sysError = System::GetLastErrorText();

			//if ( !sysError.empty() )
			//{
			//	error << cuT( "  System: " ) << sysError << std::endl;
			//}

			//l_error << Debug::Backtrace{ 20, 4 };
			//Logger::LogError( error );
			result = false;
			errorCode = m_pfnGetError();
		}

		return result;
	}

	void GlDebug::DebugLog( GlDebugSource source
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

		if ( !IsFiltered( id ) )
		{
			StringStream toLog;
			auto msg = string::string_cast< xchar >( message );
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
				Logger::LogError( toLog );
				break;

			case GlDebugSeverity::eMedium:
				Logger::LogWarning( toLog );
				break;

			case GlDebugSeverity::eLow:
				Logger::LogInfo( toLog );
				break;

			case GlDebugSeverity::eNotification:
				Logger::LogTrace( toLog );
				break;

			default:
				Logger::LogWarning( toLog );
				break;
			}
		}
	}

	void GlDebug::DebugLogAMD( uint32_t id
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
		auto msg = string::string_cast< xchar >( message );
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
			Logger::LogError( toLog );
			break;

		case GlDebugSeverity::eMedium:
			Logger::LogWarning( toLog );
			break;

		case GlDebugSeverity::eLow:
			Logger::LogInfo( toLog );
			break;

		case GlDebugSeverity::eNotification:
			Logger::LogTrace( toLog );
			break;

		default:
			Logger::LogWarning( toLog );
			break;
		}
	}
}
