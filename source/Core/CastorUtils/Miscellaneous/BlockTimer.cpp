#include "CastorUtils/Miscellaneous/BlockTimer.hpp"

#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	BlockTimer::BlockTimer( char const * file
		, char const * function
		, String name
		, uint32_t line )
		: m_file{ file }
		, m_function{ function }
		, m_name{ castor::move( name ) }
		, m_line{ line }
	{
	}

	BlockTimer::~BlockTimer()noexcept
	{
		Logger::logInfo( makeStringStream() << cuT( "BlockTimer::Exited Block : " ) << makeString( m_file.substr( m_file.find_last_of( '/' ) + 1u ) )
			<< cuT( " " ) << makeString( m_function )
			<< cuT( ":" ) << m_line
			<< cuT( " - " ) << m_name
			<< cuT( " - time: " ) << ( double( std::chrono::duration_cast< Microseconds >( m_timer.getElapsed() ).count() ) / 1000.0 ) << cuT( " ms" ) );
	}
}
