/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Core/Connection.hpp"

#include "Core/Renderer.hpp"

namespace renderer
{
	Connection::Connection( Renderer const & renderer
		, uint32_t deviceIndex
		, WindowHandle && handle )
		: m_handle{ std::move( handle ) }
	{
	}
}
