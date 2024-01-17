#include "CastorUtils/Miscellaneous/BlockTracker.hpp"

#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	BlockTracker::BlockTracker( char const * szFunction, char const * szFile, uint32_t uiLine )
		: m_strFile( string::stringCast< xchar >( szFile ) )
		, m_strFunction( string::stringCast< xchar >( szFunction ) )
		, m_uiLine( uiLine )
	{
		Logger::logDebug( makeStringStream() << cuT( "BlockTracker::Entered Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}

	BlockTracker::~BlockTracker()noexcept
	{
		Logger::logDebug( makeStringStream() << cuT( "BlockTracker::Exited Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}
}
