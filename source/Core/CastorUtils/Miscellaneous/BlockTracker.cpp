#include "CastorUtils/Miscellaneous/BlockTracker.hpp"

#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	BlockTracker::BlockTracker( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		: m_strFile( string::stringCast< xchar >( p_szFile ) )
		, m_strFunction( string::stringCast< xchar >( p_szFunction ) )
		, m_uiLine( p_uiLine )
	{
		Logger::logInfo( makeStringStream() << cuT( "BlockTracker::Entered Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}

	BlockTracker::~BlockTracker()
	{
		Logger::logInfo( makeStringStream() << cuT( "BlockTracker::Exited Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}
}
