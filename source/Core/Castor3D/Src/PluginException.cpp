#include "PluginException.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	PluginException::PluginException( bool p_critical, std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line )
		:	Exception( "Plugin loading error : " + p_description, p_file, p_function, p_line )
		,	m_critical( p_critical )
	{
		Logger::LogDebug( p_description );
	}

	PluginException::~PluginException()throw()
	{
	}
}