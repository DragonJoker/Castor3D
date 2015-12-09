/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___DX11_CONTEXT_H___
#define ___DX11_CONTEXT_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <Context.hpp>
#include <Size.hpp>
#include <algorithm>

#if defined( min )
#	undef min
#	undef max
#	undef abs
#endif

#if defined( _WIN32 )
namespace Dx11Render
{
	class DxContext
		:	public Castor3D::Context
	{
	public:
		DxContext( DxRenderSystem & p_renderSystem );
		virtual ~DxContext();

		virtual void UpdateFullScreen( bool p_value );
		virtual Castor::Size GetMaxSize( Castor::Size const & p_size )
		{
			return Castor::Size( std::min( p_size.width(), m_size.width() ), std::min( p_size.height(), m_size.height() ) );
		}
		ID3D11DeviceContext * GetDeviceContext()const
		{
			return m_pDeviceContext;
		}
		IDXGISwapChain * GetSwapChain()const
		{
			return m_swapChain;
		}

	private:
		/**
		 *\copydoc		Castor3D::Context::DoInitialise
		 */
		virtual bool DoInitialise();
		/**
		 *\copydoc		Castor3D::Context::DoCleanup
		 */
		virtual void DoCleanup();
		/**
		 *\copydoc		Castor3D::Context::DoSetCurrent
		 */
		virtual void DoSetCurrent();
		/**
		 *\copydoc		Castor3D::Context::DoEndCurrent
		 */
		virtual void DoEndCurrent();
		/**
		 *\copydoc		Castor3D::Context::DoSwapBuffers
		 */
		virtual void DoSwapBuffers();
		/**
		 *\copydoc		Castor3D::Context::DoSetAlphaFunc
		 */
		virtual void DoSetAlphaFunc( Castor3D::eALPHA_FUNC p_func, uint8_t p_value );
		/**
		 *\copydoc		Castor3D::Context::DoCullFace
		 */
		virtual void DoCullFace( Castor3D::eFACE p_eCullFace );

		void DoInitVolatileResources();
		void DoFreeVolatileResources();
		HRESULT DoInitPresentParameters();

	private:
		IDXGISwapChain * m_swapChain;
		ID3D11DeviceContext * m_pDeviceContext;
		HWND m_hWnd;
		Castor::Size m_size;
		DXGI_SWAP_CHAIN_DESC m_deviceParams;
	};
}
#endif

#endif
