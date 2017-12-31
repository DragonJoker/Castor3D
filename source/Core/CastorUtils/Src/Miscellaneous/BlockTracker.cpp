#include "BlockTracker.hpp"
#include "Log/Logger.hpp"
#include "StringUtils.hpp"

namespace castor
{
	BlockTracker::BlockTracker( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		: m_strFunction( string::stringCast< xchar >( p_szFunction ) )
		, m_strFile( string::stringCast< xchar >( p_szFile ) )
		, m_uiLine( p_uiLine )
	{
		Logger::logInfo( StringStream() << cuT( "BlockTracker::Entered Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}

	BlockTracker::~BlockTracker()
	{
		Logger::logInfo( StringStream() << cuT( "BlockTracker::Exited Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}
}
