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
#ifndef ___DX9_CONTEXT_H___
#define ___DX9_CONTEXT_H___

#include "Dx9RenderSystemPrerequisites.hpp"

#include <Context.hpp>
#include <Size.hpp>

#if defined( _WIN32 )
namespace Dx9Render
{
	class DxContext
		:	public Castor3D::Context
	{
	public:
		DxContext();
		virtual ~DxContext();

		virtual void UpdateFullScreen( bool p_bVal );
		IDirect3DSurface9 *	GetRenderTargetView()const
		{
			return m_pBackBuffer;
		}
		virtual Castor::Size GetMaxSize( Castor::Size const & p_size )
		{
			return Castor::Size( min( p_size.width(), m_size.width() ), min( p_size.height(), m_size.height() ) );
		}

	private:
		virtual bool DoInitialise();
		virtual void DoCleanup();
		virtual void DoSetCurrent();
		virtual void DoEndCurrent();
		virtual void DoSwapBuffers();
		virtual void DoSetClearColour( Castor::Colour const & p_clrClear );
		virtual void DoClear( uint32_t p_uiTargets );
		virtual void DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eMode );
		virtual void DoSetAlphaFunc( Castor3D::eALPHA_FUNC p_eFunc, uint8_t p_byValue );
		virtual void DoCullFace( Castor3D::eFACE p_eCullFace );

	private:
		IDirect3DSwapChain9 * m_pSwapChain;
		IDirect3DSurface9 * m_pBackBuffer;
		IDirect3DDevice9 * m_pDevice;
		HWND m_hWnd;
		Castor::Size m_size;
		Castor::Size m_maxSize;
		DWORD m_dwClearColour;
		static Castor3D::eFACE sm_eCullFace;
	};
}
#endif

#endif
