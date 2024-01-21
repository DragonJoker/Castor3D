#include "Castor3D/Plugin/PluginException.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

namespace castor3d
{
	PluginException::PluginException( bool critical
		, std::string const & description
		, char const * file
		, char const * function
		, uint32_t line )
		: castor::Exception( "Plugin loading error : " + description, file, function, line )
		, m_critical( critical )
	{
	}
}
