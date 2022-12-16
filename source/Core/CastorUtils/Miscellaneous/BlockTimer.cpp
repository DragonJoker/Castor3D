#include "CastorUtils/Miscellaneous/BlockTimer.hpp"

#include "CastorUtils/Log/Logger.hpp"

namespace castor
{
	BlockTimer::BlockTimer( std::string szFunction, char const * szFile, uint32_t uiLine )
		: m_strFile( string::stringCast< xchar >( szFile ) )
		, m_strFunction( string::stringCast< xchar >( szFunction ) )
		, m_uiLine( uiLine )
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
