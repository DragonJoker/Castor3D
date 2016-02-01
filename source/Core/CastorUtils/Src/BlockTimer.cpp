#include "BlockTimer.hpp"
#include "Logger.hpp"
#include "StringUtils.hpp"

namespace Castor
{
	BlockTimer::BlockTimer( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		: m_strFunction( string::string_cast< xchar >( p_szFunction ) )
		, m_strFile( string::string_cast< xchar >( p_szFile ) )
		, m_uiLine( p_uiLine )
	{
#if CASTOR_USE_TRACK
		Logger::LogInfo( StringStream() << cuT( "BlockTimer::Entered Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
#endif
	}

	BlockTimer::~BlockTimer()
	{
#if CASTOR_USE_TRACK
		Logger::LogInfo( StirngStream() << cuT( "BlockTimer::Exited Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine << cuT( " - time : " ) << m_timer.TimeS() );
#endif
	}
}
