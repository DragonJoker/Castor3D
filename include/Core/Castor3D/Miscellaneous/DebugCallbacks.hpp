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
		DebugCallbacks( DebugCallbacks const & ) = delete;
		DebugCallbacks( DebugCallbacks && )noexcept = delete;
		DebugCallbacks & operator=( DebugCallbacks const & ) = delete;
		DebugCallbacks & operator=( DebugCallbacks && )noexcept = delete;
		DebugCallbacks( ashes::Instance const & instance
			, void * userData );
		~DebugCallbacks()noexcept;

	private:
		ashes::Instance const & m_instance;
#if VK_EXT_debug_utils
		VkDebugUtilsMessengerEXT m_messenger{};
#endif
#if VK_EXT_debug_report
		VkDebugReportCallbackEXT m_callback{};
#endif
	};
	using DebugCallbacksPtr = castor::RawUniquePtr< DebugCallbacks >;
}

#endif
