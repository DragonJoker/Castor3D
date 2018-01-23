/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Core/WindowHandle.hpp"

namespace renderer
{
	WindowHandle::WindowHandle( IWindowHandlePtr handle )
		: m_handle{ std::move( handle ) }
	{
	}

	WindowHandle::operator bool()
	{
		return m_handle && m_handle->operator bool();
	}
}
