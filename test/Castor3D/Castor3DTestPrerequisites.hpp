/* See LICENSE file in root folder */
#ifndef ___C3DT_TestPrerequisites___
#define ___C3DT_TestPrerequisites___

#include <UnitTest.hpp>
#include <Benchmark.hpp>

#include <ashespp/Core/WindowHandle.hpp>
#include <ashespp/Core/PlatformWindowHandle.hpp>

#include "Castor3DTestCommon.hpp"

namespace Testing
{
	//*********************************************************************************************

	class TestWindowHandle
		: public ashes::IWindowHandle
	{
	public:
		TestWindowHandle()
			: ashes::IWindowHandle{ ashes::KHR_PLATFORM_SURFACE_EXTENSION_NAME }
#if defined( VK_USE_PLATFORM_WIN32_KHR )
			, m_mswHandle{ {}, {} }
#elif defined( VK_USE_PLATFORM_ANDROID_KHR )
			, m_androidHandle{ {} }
#elif defined( VK_USE_PLATFORM_XCB_KHR )
			, m_xcbHandle{ {}, {} }
#elif defined( VK_USE_PLATFORM_MIR_KHR )
			, m_mirHandle{ {}, {} }
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
			, m_waylandHandle{ {}, {} }
#elif defined( VK_USE_PLATFORM_XLIB_KHR )
			, m_xlibHandle{ {}, {} }
#elif defined( VK_USE_PLATFORM_MACOS_MVK )
			, m_macosHandle{ {} }
#endif
		{
		}

		operator bool()override
		{
			return true;
		}

#if defined( VK_USE_PLATFORM_WIN32_KHR )

		ashes::IMswWindowHandle m_mswHandle;

		operator ashes::IMswWindowHandle const &()const
		{
			return m_mswHandle;
		}

		operator ashes::IMswWindowHandle &()
		{
			return m_mswHandle;
		}

		ashes::IMswWindowHandle const * operator *()const
		{
			return &m_mswHandle;
		}

		ashes::IMswWindowHandle * operator *()
		{
			return &m_mswHandle;
		}

#elif defined( VK_USE_PLATFORM_ANDROID_KHR )

		ashes::IAndroidWindowHandle m_androidHandle;

		operator ashes::IAndroidWindowHandle const & ()const
		{
			return m_androidHandle;
		}

		operator ashes::IAndroidWindowHandle & ()
		{
			return m_androidHandle;
		}

		ashes::IAndroidWindowHandle const * operator *()const
		{
			return &m_androidHandle;
		}

		ashes::IAndroidWindowHandle * operator *()
		{
			return &m_androidHandle;
		}

#elif defined( VK_USE_PLATFORM_XCB_KHR )

		ashes::IXcbWindowHandle m_xcbHandle;

		operator ashes::IXcbWindowHandle const & ()const
		{
			return m_xcbHandle;
		}

		operator ashes::IXcbWindowHandle & ()
		{
			return m_xcbHandle;
		}

		ashes::IXcbWindowHandle const * operator *()const
		{
			return &m_xcbHandle;
		}

		ashes::IXcbWindowHandle * operator *()
		{
			return &m_xcbHandle;
		}

#elif defined( VK_USE_PLATFORM_MIR_KHR )

		ashes::IMirWindowHandle m_mirHandle;

		operator ashes::IMirWindowHandle const & ()const
		{
			return m_mirHandle;
		}

		operator ashes::IMirWindowHandle & ()
		{
			return m_mirHandle;
		}

		ashes::IMirWindowHandle const * operator *()const
		{
			return &m_mirHandle;
		}

		ashes::IMirWindowHandle * operator *()
		{
			return &m_mirHandle;
		}

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

		ashes::IWaylandWindowHandle m_waylandHandle;

		operator ashes::IWaylandWindowHandle const & ()const
		{
			return m_waylandHandle;
		}

		operator ashes::IWaylandWindowHandle & ()
		{
			return m_waylandHandle;
		}

		ashes::IWaylandWindowHandle const * operator *()const
		{
			return &m_waylandHandle;
		}

		ashes::IWaylandWindowHandle * operator *()
		{
			return &m_waylandHandle;
		}

#elif defined( VK_USE_PLATFORM_XLIB_KHR )

		ashes::IXWindowHandle m_xlibHandle;

		operator ashes::IXWindowHandle const & ()const
		{
			return m_xlibHandle;
		}

		operator ashes::IXWindowHandle & ()
		{
			return m_xlibHandle;
		}

		ashes::IXWindowHandle const * operator *()const
		{
			return &m_xlibHandle;
		}

		ashes::IXWindowHandle * operator *()
		{
			return &m_xlibHandle;
		}

#elif defined( VK_USE_PLATFORM_MACOS_MVK )

		ashes::IMacOsWindowHandle m_macosHandle;

		operator ashes::IMacOsWindowHandle const & ()const
		{
			return m_macosHandle;
		}

		operator ashes::IMacOsWindowHandle & ()
		{
			return m_macosHandle;
		}

		ashes::IMacOsWindowHandle const * operator *()const
		{
			return &m_macosHandle;
		}

		ashes::IMacOsWindowHandle * operator *()
		{
			return &m_macosHandle;
		}

#else

#	error "Unsupported window system."

#endif
	};

	//*********************************************************************************************
}

#endif
