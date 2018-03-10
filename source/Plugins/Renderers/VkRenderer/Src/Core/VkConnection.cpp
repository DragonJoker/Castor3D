/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/VkConnection.hpp"

#include "Core/VkPhysicalDevice.hpp"
#include "Core/VkRenderer.hpp"

#include <Core/PlatformWindowHandle.hpp>

namespace vk_renderer
{
	Connection::Connection( Renderer const & renderer
		, uint32_t deviceIndex
		, renderer::WindowHandle && handle )
		: renderer::Connection{ renderer, deviceIndex, std::move( handle ) }
		, m_renderer{ renderer }
		, m_gpu{ static_cast< PhysicalDevice const & >( renderer.getPhysicalDevice( deviceIndex ) ) }
	{
		doCreatePresentSurface();
		doRetrievePresentationInfos();
	}

	Connection::~Connection()
	{
		if ( m_presentSurface != VK_NULL_HANDLE )
		{
			m_renderer.vkDestroySurfaceKHR( m_renderer
				, m_presentSurface
				, nullptr );
		}
	}

#if RENDERLIB_WIN32

	void Connection::doCreatePresentSurface()
	{
		VkWin32SurfaceCreateInfoKHR createInfo
		{
			VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			nullptr,
			0,
			m_handle.getInternal< renderer::IMswWindowHandle >().getHinstance(),
			m_handle.getInternal< renderer::IMswWindowHandle >().getHwnd(),
		};
		DEBUG_DUMP( createInfo );
		auto res = m_renderer.vkCreateWin32SurfaceKHR( m_renderer
			, &createInfo
			, nullptr
			, &m_presentSurface );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Presentation surface creation failed: " + getLastError() };
		}
	}

#elif RENDERLIB_ANDROID

	void Connection::doCreatePresentSurface()
	{
		VkAndroidSurfaceCreateInfoKHR createInfo =
		{
			VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
			nullptr,
			0,
			m_handle.getInternal< renderer::IAndroidWindowHandle >().getWindow(),
		};
		DEBUG_DUMP( createInfo );
		auto res = m_renderer.vkCreateAndroidSurfaceKHR( m_renderer
			, &createInfo
			, nullptr
			, &m_presentSurface );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Presentation surface creation failed: " + getLastError() };
		}
	}

#elif RENDERLIB_XCB

	void Connection::doCreatePresentSurface()
	{
		VkXcbSurfaceCreateInfoKHR createInfo
		{
			VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
			nullptr,
			0,
			m_handle.getInternal< renderer::IXcbWindowHandle >().getConnection(),
			m_handle.getInternal< renderer::IXcbWindowHandle >().getHandle(),
		};
		auto res = m_renderer.vkCreateXcbSurfaceKHR( m_renderer
			, &createInfo
			, nullptr
			, &m_presentSurface );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Presentation surface creation failed: " + getLastError() };
		}
	}

#elif RENDERLIB_MIR

	void Connection::doCreatePresentSurface()
	{
		VkMirSurfaceCreateInfoKHR createInfo =
		{
			VK_STRUCTURE_TYPE_MIR_SURFACE_CREATE_INFO_KHR,
			nullptr,
			flags,
			m_handle.getInternal< renderer::IMirWindowHandle >().getConnection(),
			m_handle.getInternal< renderer::IMirWindowHandle >().getSurface(),
		};
		auto res = m_renderer.vkCreateMirSurfaceKHR( m_renderer
			, &createInfo
			, nullptr
			, &m_presentSurface );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Presentation surface creation failed: " + getLastError() };
		}
	}

#elif RENDERLIB_WAYLAND

	void Connection::doCreatePresentSurface()
	{
		VkWaylandSurfaceCreateInfoKHR createInfo =
		{
			VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
			nullptr,
			0,
			m_handle.getInternal< renderer::IWaylandWindowHandle >().getDisplay(),
			m_handle.getInternal< renderer::IWaylandWindowHandle >().getSurface(),
		};
		auto res = m_renderer.vkCreateWaylandSurfaceKHR( m_renderer
			, &createInfo
			, nullptr
			, &m_presentSurface );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Presentation surface creation failed: " + getLastError() };
		}
	}

#elif RENDERLIB_XLIB

	void Connection::doCreatePresentSurface()
	{
		VkXlibSurfaceCreateInfoKHR createInfo
		{
			VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
			nullptr,
			0,
			m_handle.getInternal< renderer::IXWindowHandle >().getDisplay(),
			m_handle.getInternal< renderer::IXWindowHandle >().getDrawable(),
		};
		auto res = m_renderer.vkCreateXlibSurfaceKHR( m_renderer
			, &createInfo
			, nullptr
			, &m_presentSurface );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Presentation surface creation failed: " + getLastError() };
		}
	}

#else

#	error "Unsupported window system."

#endif

	VkSurfaceCapabilitiesKHR Connection::getSurfaceCapabilities()const
	{
		// On récupère les capacités de la surface.
		VkSurfaceCapabilitiesKHR caps;
		auto res = m_renderer.vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_gpu
			, m_presentSurface
			, &caps );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Surface capabilities check failed: " + getLastError() };
		}

		return caps;
	}

	void Connection::doRetrievePresentationInfos()
	{
		// Parcours des propriétés des files, pour vérifier leur support de la pr�sentation.
		std::vector< VkBool32 > supportsPresent( static_cast< uint32_t >( m_gpu.getQueueProperties().size() ) );
		uint32_t i{ 0u };
		m_graphicsQueueFamilyIndex = std::numeric_limits< uint32_t >::max();
		m_presentQueueFamilyIndex = std::numeric_limits< uint32_t >::max();
		m_computeQueueFamilyIndex = std::numeric_limits< uint32_t >::max();

		for ( auto & present : supportsPresent )
		{
			m_renderer.vkGetPhysicalDeviceSurfaceSupportKHR( m_gpu
				, i
				, m_presentSurface
				, &present );

			if ( m_gpu.getQueueProperties()[i].queueCount > 0 )
			{
				if ( m_gpu.getQueueProperties()[i].queueFlags & renderer::QueueFlag::eGraphics )
				{
					// Tout d'abord on choisit une file graphique
					if ( m_graphicsQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
					{
						m_graphicsQueueFamilyIndex = i;
					}

					// Si elle supporte aussi les calculs, on l'utilisera aussi en tant que file de calcul.
					if ( m_gpu.getQueueProperties()[i].queueFlags & renderer::QueueFlag::eCompute
						&& m_computeQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
					{
						m_computeQueueFamilyIndex = i;
					}

					// Si une file supporte les graphismes et la présentation, on la préfère.
					if ( present )
					{
						m_graphicsQueueFamilyIndex = i;
						m_presentQueueFamilyIndex = i;
						break;
					}
				}

				if ( m_gpu.getQueueProperties()[i].queueFlags & renderer::QueueFlag::eCompute
					&& m_computeQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
				{
					m_computeQueueFamilyIndex = i;
				}
			}

			++i;
		}

		if ( m_presentQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
		{
			// Pas de file supportant les deux, on a donc 2 files distinctes.
			for ( size_t i = 0; i < m_gpu.getQueueProperties().size(); ++i )
			{
				if ( supportsPresent[i] )
				{
					m_presentQueueFamilyIndex = static_cast< uint32_t >( i );
					break;
				}
			}
		}

		// Si on n'en a pas trouvé, on génère une erreur.
		if ( m_graphicsQueueFamilyIndex == std::numeric_limits< uint32_t >::max()
			|| m_presentQueueFamilyIndex == std::numeric_limits< uint32_t >::max()
			|| m_computeQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
		{
			throw std::runtime_error{ "Could not find appropriate queue families" };
		}
	}
}
