#include "BlockTimer.hpp"
#include "Log/Logger.hpp"

namespace Castor
{
	BlockTimer::BlockTimer( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		: m_strFunction( string::string_cast< xchar >( p_szFunction ) )
		, m_strFile( string::string_cast< xchar >( p_szFile ) )
		, m_uiLine( p_uiLine )
	{
	}

	BlockTimer::~BlockTimer()
	{
		Logger::LogInfo( StringStream() << cuT( "BlockTimer::Exited Block : " ) << m_strFunction
			<< cuT( " in " ) << m_strFile
			<< cuT( ", line " ) << m_uiLine
			<< cuT( " - time: " ) << std::chrono::duration_cast< Castor::Milliseconds >( m_timer.Time() ).count() << cuT( " ms" ) );
	}
}
