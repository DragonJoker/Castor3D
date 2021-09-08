#include "CastorUtils/Miscellaneous/BlockTimer.hpp"

#include "CastorUtils/Log/Logger.hpp"

namespace castor
{
	BlockTimer::BlockTimer( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		: m_strFile( string::stringCast< xchar >( p_szFile ) )
		, m_strFunction( string::stringCast< xchar >( p_szFunction ) )
		, m_uiLine( p_uiLine )
	{
	}

	BlockTimer::~BlockTimer()
	{
		Logger::logInfo( makeStringStream() << cuT( "BlockTimer::Exited Block : " ) << m_strFunction
			<< cuT( " in " ) << m_strFile
			<< cuT( ", line " ) << m_uiLine
			<< cuT( " - time: " ) << std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() ).count() << cuT( " ms" ) );
	}
}
