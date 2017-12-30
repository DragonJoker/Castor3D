#include "VersionException.hpp"

using namespace castor;

namespace castor3d
{
	VersionException::VersionException( Version const & p_given, Version const & p_expected, char const * p_strFile, char const * p_strFunction, uint32_t p_uiLine )
		:	Exception( "", p_strFile, p_strFunction, p_uiLine )
	{
		std::stringstream stream;
		stream << "Expected version [";
		stream << p_expected;
		stream << "] but got version [";
		stream << p_given;
		stream << "] - Incompatible versions";
		m_description = stream.str();
		Logger::logDebug( m_description );
	}
}
