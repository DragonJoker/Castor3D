/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugCallbacks_H___
#define ___C3D_DebugCallbacks_H___

#include "MiscellaneousModule.hpp"

#include <memory>

namespace castor3d
{
	class DebugCallbacks
	{
	public:
		DebugCallbacks( ashes::Instance const & instance
			, void * userData );
		~DebugCallbacks();

	private:
		ashes::Instance const & m_instance;
#if VK_EXT_debug_utils
		VkDebugUtilsMessengerEXT m_messenger{ nullptr };
#endif
#if VK_EXT_debug_report
		VkDebugReportCallbackEXT m_callback{ nullptr };
#endif
	};
	using DebugCallbacksPtr = std::unique_ptr< DebugCallbacks >;
}

#endif
