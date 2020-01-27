#include "Castor3D/Plugin/PluginException.hpp"

using namespace castor;

namespace castor3d
{
	PluginException::PluginException( bool critical
		, std::string const & description
		, char const * file
		, char const * function
		, uint32_t line )
		: Exception( "Plugin loading error : " + description, file, function, line )
		, m_critical( critical )
	{
		Logger::logDebug( description );
	}

	PluginException::~PluginException()throw()
	{
	}
}
