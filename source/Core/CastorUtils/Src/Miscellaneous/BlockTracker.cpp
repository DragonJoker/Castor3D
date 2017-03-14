#include "BlockTracker.hpp"
#include "Log/Logger.hpp"
#include "StringUtils.hpp"

namespace Castor
{
	BlockTracker::BlockTracker( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine )
		: m_strFunction( string::string_cast< xchar >( p_szFunction ) )
		, m_strFile( string::string_cast< xchar >( p_szFile ) )
		, m_uiLine( p_uiLine )
	{
		Logger::LogInfo( StringStream() << cuT( "BlockTracker::Entered Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}

	BlockTracker::~BlockTracker()
	{
		Logger::LogInfo( StringStream() << cuT( "BlockTracker::Exited Block : " ) << m_strFunction << cuT( " in " ) << m_strFile << cuT( ", line " ) << m_uiLine );
	}
}
