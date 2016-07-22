#include "VersionException.hpp"
#include "Version.hpp"

#include <Log/Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	VersionException::VersionException( Version const & p_given, Version const & p_expected, char const * p_strFile, char const * p_strFunction, uint32_t p_uiLine )
		:	Exception( "", p_strFile, p_strFunction, p_uiLine )
	{
		std::stringstream l_stream;
		l_stream << "Expected version [";
		l_stream << p_expected.m_iMajor;
		l_stream << ".";
		l_stream << p_expected.m_iMinor;
		l_stream << ".";
		l_stream << p_expected.m_iBuild;
		l_stream << "] but got version [";
		l_stream << p_given.m_iMajor;
		l_stream << ".";
		l_stream << p_given.m_iMinor;
		l_stream << ".";
		l_stream << p_given.m_iBuild;
		l_stream << "] - Incompatible versions";
		m_description = l_stream.str();
		Logger::LogDebug( m_description );
	}
}