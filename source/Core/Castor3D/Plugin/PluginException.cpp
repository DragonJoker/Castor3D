#include "Castor3D/Plugin/PluginException.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

namespace c3d
{
	PluginException::PluginException( bool critical
		, MbString const & description
		, char const * file
		, char const * function
		, uint32_t line )
		: Exception( "Plugin loading error : " + description, file, function, line )
		, m_critical( critical )
	{
	}
}
