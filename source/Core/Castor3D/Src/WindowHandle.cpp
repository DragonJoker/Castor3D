#include "WindowHandle.hpp"

using namespace Castor;

namespace Castor3D
{
	WindowHandle::WindowHandle()
		: m_pHandle()
	{
	}

	WindowHandle::WindowHandle( IWindowHandleSPtr p_pHandle )
		: m_pHandle( p_pHandle )
	{
	}

	WindowHandle::WindowHandle( WindowHandle const & p_handle )
		: m_pHandle( p_handle.m_pHandle )
	{
	}

	WindowHandle::WindowHandle( WindowHandle && p_handle )
		: m_pHandle( std::move( p_handle.m_pHandle ) )
	{
		p_handle.m_pHandle.reset();
	}

	WindowHandle::~WindowHandle()
	{
	}

	WindowHandle & WindowHandle::operator =( WindowHandle const & p_handle )
	{
		m_pHandle = p_handle.m_pHandle;
		return *this;
	}

	WindowHandle & WindowHandle::operator =( WindowHandle && p_handle )
	{
		if ( this != &p_handle )
		{
			m_pHandle = std::move( p_handle.m_pHandle );
			p_handle.m_pHandle.reset();
		}

		return *this;
	}

	WindowHandle::operator bool()
	{
		return m_pHandle && m_pHandle->operator bool();
	}
}
