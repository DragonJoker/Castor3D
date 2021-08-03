/*
See LICENSE file in root folder
*/
#ifndef ___CU_LoggerStreambuf_H___
#define ___CU_LoggerStreambuf_H___

#include "CastorUtils/Log/LoggerInstance.hpp"
#include "CastorUtils/Config/MultiThreadConfig.hpp"

#include <mutex>

namespace castor
{
	template< typename CharT, typename TraitsT >
	class LoggerStreambufT
		: public std::basic_streambuf< CharT >
	{
	public:
		using string_type = std::basic_string< CharT >;
		using ostream_type = std::basic_ostream< CharT >;
		using streambuf_type = std::basic_streambuf< CharT >;
		using int_type = typename std::basic_streambuf< CharT >::int_type;
		using traits_type = typename std::basic_streambuf< CharT >::traits_type;

		explicit LoggerStreambufT( std::basic_ostream< CharT > & p_stream )
			: m_stream( p_stream )
			, m_logger{ nullptr }
		{
			m_old = m_stream.rdbuf( this );
		}
		
		explicit LoggerStreambufT( LoggerInstance & logger
			, ostream_type & stream )
			: m_stream( stream )
			, m_logger{ &logger }
		{
			m_old = m_stream.rdbuf( this );
		}

		~LoggerStreambufT()noexcept
		{
			doSync();
			m_stream.rdbuf( m_old );
		}

		int_type overflow( int_type c = traits_type::eof() )override
		{
			if ( !m_logger )
			{
				return c;
			}

			auto lock( makeUniqueLock( *m_logger ) );
			{
				if ( traits_type::eq_int_type( c, traits_type::eof() ) )
				{
					doSync();
				}
				else if ( c == '\n' )
				{
					doSync();
				}
				else if ( c == '\r' )
				{
					m_buffer += '\r';
					doSyncNoLF();
				}
				else
				{
					m_buffer += traits_type::to_char_type( c );
				}
			}

			return c;
		}

	private:
		int doSync()
		{
			if ( !m_buffer.empty() )
			{
				if ( m_logger )
				{
					TraitsT::log( *m_logger, m_buffer );
				}

				m_buffer.clear();
			}

			return 0;
		}

		int doSyncNoLF()
		{
			if ( !m_buffer.empty() )
			{
				TraitsT::logNoLF( *m_logger, m_buffer );
				m_buffer.clear();
			}

			return 0;
		}

	private:
		ostream_type & m_stream;
		LoggerInstance * m_logger;
		streambuf_type * m_old;
		string_type m_buffer;
	};
	/**
	\~english
	\brief		Streambuf traits for trace logging.
	\~french
	\brief		Traits de streambuf pour du log de trace.
	*/
	template< typename CharType >
	struct TraceLoggerStreambufTraitsT
	{
		static void log( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogTrace( string::stringCast< char >( text ) );
		}
		static void logNoLF( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogTraceNoLF( string::stringCast< char >( text ) );
		}
	};
	/**
	\~english
	\brief		Streambuf traits for debug logging.
	\~french
	\brief		Traits de streambuf pour du log de debug.
	*/
	template< typename CharType >
	struct DebugLoggerStreambufTraitsT
	{
		static void log( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogDebug( string::stringCast< char >( text ) );
		}
		static void logNoLF( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogDebugNoLF( string::stringCast< char >( text ) );
		}
	};
	/**
	\~english
	\brief		Streambuf traits for info logging.
	\~french
	\brief		Traits de streambuf pour du log d'info.
	*/
	template< typename CharType >
	struct InfoLoggerStreambufTraitsT
	{
		static void log( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogInfo( string::stringCast< char >( text ) );
		}
		static void logNoLF( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogInfoNoLF( string::stringCast< char >( text ) );
		}
	};
	/**
	\~english
	\brief		Streambuf traits for warning logging.
	\~french
	\brief		Traits de streambuf pour du log de warning.
	*/
	template< typename CharType >
	struct WarningLoggerStreambufTraitsT
	{
		static void log( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogWarning( string::stringCast< char >( text ) );
		}
		static void logNoLF( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogWarningNoLF( string::stringCast< char >( text ) );
		}
	};
	/**
	\~english
	\brief		Streambuf traits for error logging.
	\~french
	\brief		Traits de streambuf pour du log d'erreur.
	*/
	template< typename CharType >
	struct ErrorLoggerStreambufTraitsT
	{
		static void log( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogError( string::stringCast< char >( text ) );
		}
		static void logNoLF( LoggerInstance & logger
			, std::basic_string< CharType > const & text )
		{
			logger.lockedLogErrorNoLF( string::stringCast< char >( text ) );
		}
	};

	template< typename CharT >
	using TraceLoggerStreambufT = LoggerStreambufT< CharT, TraceLoggerStreambufTraitsT< CharT > >;
	template< typename CharT >
	using DebugLoggerStreambufT = LoggerStreambufT< CharT, DebugLoggerStreambufTraitsT< CharT > >;
	template< typename CharT >
	using InfoLoggerStreambufT = LoggerStreambufT< CharT, InfoLoggerStreambufTraitsT< CharT > >;
	template< typename CharT >
	using WarningLoggerStreambufT = LoggerStreambufT< CharT, WarningLoggerStreambufTraitsT< CharT > >;
	template< typename CharT >
	using ErrorLoggerStreambufT = LoggerStreambufT< CharT, ErrorLoggerStreambufTraitsT< CharT > >;
}

#endif
