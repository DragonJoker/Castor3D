#include "VkRendererPrerequisites.hpp"

#include <algorithm>
#include <cstring>

namespace vk_renderer
{
	void checkExtensionsAvailability( std::vector< VkExtensionProperties > const & available
		, std::vector< char const * > const & requested )
	{
		for ( auto const & name : requested )
		{
			if ( available.end() == std::find_if( available.begin()
				, available.end()
				, [&name]( auto const & extension )
				{
					return strcmp( extension.extensionName, name ) == 0;
				} ) )
			{
				throw std::runtime_error{ "Extension " + std::string( name ) + " is not supported." };
			}
		}
	}
}
