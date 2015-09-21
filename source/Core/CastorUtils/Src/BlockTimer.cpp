#include "BlockTimer.hpp"
#include "Logger.hpp"

namespace Castor
{
	BlockTimer::BlockTimer( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		: m_strFunction( string::string_cast< xchar >( p_szFunction ) )
		, m_strFile( string::string_cast< xchar >( p_szFile ) )
		, m_uiLine( p_uiLine	)
	{
#if CASTOR_USE_TRACK
#	if CASTOR_USE_LOG_TRACK
		String l_strToLog;
		Logger::LogInfo( l_strToLog + cuT( "BlockTimer::Entered Block : " ) + m_strFunction + cuT( " in " ) + m_strFile + cuT( ", line " ) + string::to_string( m_uiLine ) );
#	else
		std::cout << "BlockTimer::Entered Block : " << m_strFunction << " in " << m_strFile << ", line " << m_uiLine << std::endl;
#	endif
#endif
	}

	BlockTimer::~BlockTimer()
	{
#if CASTOR_USE_TRACK
#	if CASTOR_USE_LOG_TRACK
		String l_strToLog;
		Logger::LogInfo( l_strToLog + cuT( "BlockTimer::Exited Block : " ) + m_strFunction + cuT( " in " ) + m_strFile + cuT( ", line " ) + string::to_string( m_uiLine ) + cuT( " - time : " ) + string::to_string( m_timer.TimeS() ) );
#	else
		std::cout << "BlockTimer::Exited Block : " << m_strFunction << " in " << m_strFunction << ", line " << m_uiLine << " - time : " << m_timer.Time() << std::endl;
#	endif
#endif
	}
}
