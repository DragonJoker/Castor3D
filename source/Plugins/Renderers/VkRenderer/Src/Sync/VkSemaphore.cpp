/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Sync/VkSemaphore.hpp"

#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	Semaphore::Semaphore( Device const & device )
		: renderer::Semaphore{ device }
		, m_device{ device }
	{
		VkSemaphoreCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			nullptr,
			0                                         // flags
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateSemaphore( m_device, &createInfo, nullptr, &m_semaphore );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Semaphore creation failed: " + getLastError() };
		}
	}

	Semaphore::~Semaphore()
	{
		vk::DestroySemaphore( m_device, m_semaphore, nullptr );
	}
}
