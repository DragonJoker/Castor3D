#include "BlockTracker.hpp"
#include "Logger.hpp"
#include "StringUtils.hpp"

namespace Castor
{
	BlockTracker::BlockTracker( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		:	m_strFunction( string::string_cast< xchar >( p_szFunction )	)
		,	m_strFile( string::string_cast< xchar >( p_szFile )	)
		,	m_uiLine( p_uiLine	)
	{
#if CASTOR_USE_TRACK
#	if CASTOR_USE_LOG_TRACK
		String l_strToLog;
		Logger::LogInfo( l_strToLog + cuT( "BlockTracker::Entered Block : " ) + m_strFunction + cuT( " in " ) + m_strFile + cuT( ", line " ) + string::to_string( m_uiLine ) );
#	else
		std::cout << "BlockTracker::Entered Block : " << m_strFunction << " in " << m_strFile << ", line " << m_uiLine << std::endl;
#	endif
#endif
	}

	BlockTracker::~BlockTracker()
	{
#if CASTOR_USE_TRACK
#	if CASTOR_USE_LOG_TRACK
		String l_strToLog;
		Logger::LogInfo( l_strToLog + cuT( "BlockTracker::Exited Block : " ) + m_strFunction + cuT( " in " ) + m_strFile + cuT( ", line " ) + string::to_string( m_uiLine ) );
#	else
		std::cout << "BlockTracker::Exited Block : " << m_strFunction << " in " << m_strFile << ", line " << m_uiLine << std::endl;
#	endif
#endif
	}
}
