#include "Miscellaneous/GlES3Debug.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"

#include <map>
#include <iomanip>

#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#	define GL_INVALID_FRAMEBUFFER_OPERATION 0x506
#endif

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3Debug::GlES3Debug( GlES3RenderSystem & p_renderSystem )
		: Holder{ p_renderSystem.GetOpenGlES3() }
		, m_renderSystem{ p_renderSystem }
	{
		m_pfnGetError = &glGetError;
	}

	GlES3Debug::~GlES3Debug()
	{
	}

	void GlES3Debug::Initialise()
	{
		auto & l_gl = GetOpenGlES3();
		FilterMessage( 0x00020072 );
		// TODO: Investigate on why I get this noisy message
		FilterMessage( 0x00020096 );
	}

	void GlES3Debug::Cleanup()
	{
		m_pfnGetError = nullptr;
		m_pfnDebugMessageCallback = nullptr;
		m_pfnDebugMessageCallbackAMD = nullptr;
	}

	bool GlES3Debug::GlES3CheckError( std::string const & p_text )const
	{
		return DoGlES3CheckError( string::string_cast< xchar >( p_text ) );
	}

	bool GlES3Debug::GlES3CheckError( std::wstring const & p_text )const
	{
		return DoGlES3CheckError( string::string_cast< xchar >( p_text ) );
	}

	void GlES3Debug::FilterMessage( uint32_t p_message )
	{
		m_filteredOut.insert( p_message );
	}

	bool GlES3Debug::IsFiltered( uint32_t p_message )const
	{
		return m_filteredOut.find( p_message ) != m_filteredOut.end();
	}

	void GlES3Debug::BindTexture( uint32_t p_name, uint32_t p_index )const
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

	void GlES3Debug::BindSampler( uint32_t p_name, uint32_t p_index )const
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

	void GlES3Debug::CheckTextureUnits()const
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

	void GlES3Debug::DoUpdateTextureUnits()const
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

	bool GlES3Debug::Track( void * p_object
		, std::string const & p_name
		, std::string const & p_file
		, int p_line )const
	{
		return m_renderSystem.Track( p_object, p_name, p_file, p_line );
	}

	bool GlES3Debug::UnTrack( void * p_object )const
	{
		return m_renderSystem.Untrack( p_object );
	}

#endif

	void GlES3Debug::StDebugLog( GlES3DebugSource source
		, GlES3DebugType type
		, uint32_t id
		, GlES3DebugSeverity severity
		, int length
		, const char * message
		, void * userParam )
	{
		reinterpret_cast< GlES3Debug * >( userParam )->DebugLog( source, type, id, severity, length, message );
	}

	void GlES3Debug::StDebugLogAMD( uint32_t id
		, GlES3DebugCategory category
		, GlES3DebugSeverity severity
		, int length
		, const char * message
		, void * userParam )
	{
		reinterpret_cast< GlES3Debug * >( userParam )->DebugLogAMD( id, category, severity, length, message );
	}

	bool GlES3Debug::DoGlES3CheckError( String const & p_text )const
	{
		static std::map< uint32_t, String > const Errors
		{
			{ GL_INVALID_ENUM, cuT( "Invalid Enum" ) },
			{ GL_INVALID_VALUE, cuT( "Invalid Value" ) },
			{ GL_INVALID_OPERATION, cuT( "Invalid Operation" ) },
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

	void GlES3Debug::DebugLog( GlES3DebugSource source
		, GlES3DebugType type
		, uint32_t id
		, GlES3DebugSeverity severity
		, int CU_PARAM_UNUSED( length )
		, const char * message )const
	{
		static std::map< GlES3DebugSource, String > SourceName
		{
			{ GlES3DebugSource::eAPI, cuT( "OpenGlES3" ) },
			{ GlES3DebugSource::eWindowSystem, cuT( "Window System" ) },
			{ GlES3DebugSource::eShaderCompiler, cuT( "Shader compiler" ) },
			{ GlES3DebugSource::eThirdParty, cuT( "Third party" ) },
			{ GlES3DebugSource::eApplication, cuT( "Application" ) },
			{ GlES3DebugSource::eOther, cuT( "Other" ) },
		};

		static std::map< GlES3DebugType, String > TypeName
		{
			{ GlES3DebugType::eError, cuT( "Error" ) },
			{ GlES3DebugType::eDeprecatedBehavior, cuT( "Deprecated behavior" ) },
			{ GlES3DebugType::eUndefinedBehavior, cuT( "Undefined behavior" ) },
			{ GlES3DebugType::ePortability, cuT( "Portability" ) },
			{ GlES3DebugType::ePerformance, cuT( "Performance" ) },
			{ GlES3DebugType::eOther, cuT( "Other" ) },
			{ GlES3DebugType::eMarker, cuT( "Marker" ) },
			{ GlES3DebugType::ePushGroup, cuT( "Push Group" ) },
			{ GlES3DebugType::ePopGroup, cuT( "Pop Group" ) },
		};

		static std::map< GlES3DebugSeverity, String > SeverityName
		{
			{ GlES3DebugSeverity::eHigh, cuT( "High" ) },
			{ GlES3DebugSeverity::eMedium, cuT( "Medium" ) },
			{ GlES3DebugSeverity::eLow, cuT( "Low" ) },
			{ GlES3DebugSeverity::eNotification, cuT( "Notification" ) },
		};

		if ( !IsFiltered( id ) )
		{
			StringStream l_toLog;
			auto l_message = string::string_cast< xchar >( message );
			string::replace( l_message, '\n', ' ' );
			l_toLog << cuT( "OpenGlES3 Debug\n " );
			l_toLog << cuT( "  Source: " ) << SourceName[source] << cuT( "\n" );
			l_toLog << cuT( "  Type: " ) << TypeName[type] << cuT( "\n" );
			l_toLog << cuT( "  Severity: " ) << SeverityName[severity] << cuT( "\n" );
			l_toLog << cuT( "  ID: 0x" ) << std::hex << std::setfill( '0' ) << std::setw( 8 ) << id << cuT( "\n" );
			l_toLog << cuT( "  Message: " ) << l_message;

			switch ( severity )
			{
			case GlES3DebugSeverity::eHigh:
				l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
				Logger::LogError( l_toLog );
				break;

			case GlES3DebugSeverity::eMedium:
				Logger::LogWarning( l_toLog );
				break;

			case GlES3DebugSeverity::eLow:
				Logger::LogInfo( l_toLog );
				break;

			case GlES3DebugSeverity::eNotification:
				Logger::LogTrace( l_toLog );
				break;

			default:
				Logger::LogWarning( l_toLog );
				break;
			}
		}
	}

	void GlES3Debug::DebugLogAMD( uint32_t id
		, GlES3DebugCategory category
		, GlES3DebugSeverity severity
		, int CU_PARAM_UNUSED( length )
		, const char * message )const
	{
		static std::map< GlES3DebugCategory, String > CategoryName
		{
			{ GlES3DebugCategory::eAPIError, cuT( "OpenGlES3" ) },
			{ GlES3DebugCategory::eWindowSystem, cuT( "Window System" ) },
			{ GlES3DebugCategory::eDeprecation, cuT( "Deprecated Behavior" ) },
			{ GlES3DebugCategory::eUndefinedBehavior, cuT( "Undefined Behavior" ) },
			{ GlES3DebugCategory::ePerformance, cuT( "Performance" ) },
			{ GlES3DebugCategory::eShaderCompiler, cuT( "Shader Compiler" ) },
			{ GlES3DebugCategory::eApplication, cuT( "Application" ) },
			{ GlES3DebugCategory::eOther, cuT( "Other" ) }
		};

		static std::map< GlES3DebugSeverity, String > SeverityName
		{
			{ GlES3DebugSeverity::eHigh, cuT( "High" ) },
			{ GlES3DebugSeverity::eMedium, cuT( "Medium" ) },
			{ GlES3DebugSeverity::eLow, cuT( "Low" ) },
			{ GlES3DebugSeverity::eNotification, cuT( "Notification" ) }
		};

		StringStream l_toLog;
		auto l_message = string::string_cast< xchar >( message );
		string::replace( l_message, '\n', ' ' );
		l_toLog << cuT( "OpenGlES3 Debug\n" );
		l_toLog << cuT( "  Category: " ) << CategoryName[category] << cuT( "\n" );
		l_toLog << cuT( "  Severity: " ) << SeverityName[severity] << cuT( "\n" );
		l_toLog << cuT( "  ID: 0x" ) << std::hex << std::setfill( '0' ) << std::setw( 8 ) << id << cuT( "\n" );
		l_toLog << cuT( "  Message: " ) << l_message;

		switch ( severity )
		{
		case GlES3DebugSeverity::eHigh:
			l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
			Logger::LogError( l_toLog );
			break;

		case GlES3DebugSeverity::eMedium:
			Logger::LogWarning( l_toLog );
			break;

		case GlES3DebugSeverity::eLow:
			Logger::LogInfo( l_toLog );
			break;

		case GlES3DebugSeverity::eNotification:
			Logger::LogTrace( l_toLog );
			break;

		default:
			Logger::LogWarning( l_toLog );
			break;
		}
	}
}
