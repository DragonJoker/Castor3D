#include "Castor3D/Miscellaneous/VersionException.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor;

namespace castor3d
{
	VersionException::VersionException( Version const & given, Version const & expected, char const * strFile, char const * strFunction, uint32_t uiLine )
		:	Exception( "", strFile, strFunction, uiLine )
	{
		std::stringstream stream;
		stream << "Expected version [";
		stream << expected;
		stream << "] but got version [";
		stream << given;
		stream << "] - Incompatible versions";
		m_description = stream.str();
		log::debug << m_description << std::endl;
	}
}
