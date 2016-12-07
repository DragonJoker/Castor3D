#include "Miscellaneous/GlDebug.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <map>

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

		FilterMessage( 0x00020072 );
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
		auto l_it = m_textureUnits.find( p_index );

		if ( !p_name )
		{
			if ( l_it == m_textureUnits.end()
				|| l_it->second.m_texture.m_name == 0u )
			{
				Logger::LogError( std::stringstream{} << "Double texture unbind for index " << p_index );
			}
		}
		else if ( l_it != m_textureUnits.end()
			&& l_it->second.m_texture.m_name != 0u )
		{
			Logger::LogError( std::stringstream{} << "Previous texture " << l_it->second.m_texture.m_name << " at index " << p_index << " has not been unbound " );
		}

		m_textureUnits[p_index].m_texture = Binding{ p_name };

		if ( !p_name )
		{
			DoUpdateTextureUnits();
		}
	}

	void GlDebug::BindSampler( uint32_t p_name, uint32_t p_index )const
	{
		auto l_it = m_textureUnits.find( p_index );

		if ( !p_name )
		{
			if ( l_it == m_textureUnits.end()
				|| l_it->second.m_sampler.m_name == 0u )
			{
				Logger::LogError( std::stringstream{} << "Double sampler unbind for index " << p_index );
			}
		}
		else if ( l_it != m_textureUnits.end()
			&& l_it->second.m_sampler.m_name != 0u )
		{
			Logger::LogError( std::stringstream{} << "Previous sampler " << l_it->second.m_sampler.m_name << " at index " << p_index << " has not been unbound " );
		}

		m_textureUnits[p_index].m_sampler = Binding{ p_name };

		if ( !p_name )
		{
			DoUpdateTextureUnits();
		}
	}

	void GlDebug::CheckTextureUnits()const
	{
		for ( auto & l_it : m_textureUnits )
		{
			if ( !l_it.second.m_sampler.m_name
				|| !l_it.second.m_texture.m_name )
			{
				Logger::LogError( std::stringstream{} << "Texture unit at index " << l_it.first << " is incomplete" );
			}
		}
	}

	void GlDebug::DoUpdateTextureUnits()const
	{
		auto l_it = m_textureUnits.begin();

		while ( l_it != m_textureUnits.end() )
		{
			if ( !l_it->second.m_sampler.m_name
				&& !l_it->second.m_texture.m_name )
			{
				l_it = m_textureUnits.erase( l_it );
			}
			else
			{
				++l_it;
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
			StringStream l_toLog;
			auto l_message = string::string_cast< xchar >( message );
			string::replace( l_message, '\n', ' ' );
			l_toLog << cuT( "OpenGl Debug\n " );
			l_toLog << cuT( "  Source: " ) << SourceName[source] << cuT( "\n" );
			l_toLog << cuT( "  Type: " ) << TypeName[type] << cuT( "\n" );
			l_toLog << cuT( "  Severity: " ) << SeverityName[severity] << cuT( "\n" );
			l_toLog << cuT( "  ID: 0x" ) << std::hex << std::setfill( '0' ) << std::setw( 8 ) << id << cuT( "\n" );
			l_toLog << cuT( "  Message: " ) << l_message;

			switch ( severity )
			{
			case GlDebugSeverity::eHigh:
				l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
				Logger::LogError( l_toLog );
				break;

			case GlDebugSeverity::eMedium:
				Logger::LogWarning( l_toLog );
				break;

			case GlDebugSeverity::eLow:
				Logger::LogInfo( l_toLog );
				break;

			case GlDebugSeverity::eNotification:
				Logger::LogTrace( l_toLog );
				break;

			default:
				Logger::LogWarning( l_toLog );
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

		StringStream l_toLog;
		auto l_message = string::string_cast< xchar >( message );
		string::replace( l_message, '\n', ' ' );
		l_toLog << cuT( "OpenGl Debug\n" );
		l_toLog << cuT( "  Category: " ) << CategoryName[category] << cuT( "\n" );
		l_toLog << cuT( "  Severity: " ) << SeverityName[severity] << cuT( "\n" );
		l_toLog << cuT( "  ID: 0x" ) << std::hex << std::setfill( '0' ) << std::setw( 8 ) << id << cuT( "\n" );
		l_toLog << cuT( "  Message: " ) << l_message;

		switch ( severity )
		{
		case GlDebugSeverity::eHigh:
			l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
			Logger::LogError( l_toLog );
			break;

		case GlDebugSeverity::eMedium:
			Logger::LogWarning( l_toLog );
			break;

		case GlDebugSeverity::eLow:
			Logger::LogInfo( l_toLog );
			break;

		case GlDebugSeverity::eNotification:
			Logger::LogTrace( l_toLog );
			break;

		default:
			Logger::LogWarning( l_toLog );
			break;
		}
	}
}
