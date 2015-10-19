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
#ifndef ___DX11_RENDER_BUFFER_H___
#define ___DX11_RENDER_BUFFER_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <Size.hpp>

#if defined( _WIN32 )
namespace Dx11Render
{
	class DxRenderBuffer
		:	public Castor::NonCopyable
	{
	public:
		DxRenderBuffer( DxRenderSystem * p_renderSystem, DXGI_FORMAT p_eFormat, Castor3D::eBUFFER_COMPONENT p_eComponent, Castor3D::RenderBuffer & p_renderBuffer );
		~DxRenderBuffer();

		bool Create();
		void Destroy();

		bool Initialise( Castor::Size const & p_size );
		void Cleanup();

		bool Resize( Castor::Size const & p_size );

		inline ID3D11View * GetResourceView()const
		{
			return m_pSurface;
		}

		inline ID3D11Texture2D * GetResource()const
		{
			return m_pTexture;
		}

	private:
		Castor::Size m_size;
		ID3D11View * m_pSurface;
		ID3D11View * m_pOldSurface;
		DxRenderSystem * m_renderSystem;
		DXGI_FORMAT m_eFormat;
		Castor3D::eBUFFER_COMPONENT m_eComponent;
		Castor3D::RenderBuffer & m_renderBuffer;
		ID3D11Texture2D * m_pTexture;
	};
}
#endif

#endif
