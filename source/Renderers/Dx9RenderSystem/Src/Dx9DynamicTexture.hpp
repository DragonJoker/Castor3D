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
#ifndef ___DX9_DYNAMIC_TEXTURE_H___
#define ___DX9_DYNAMIC_TEXTURE_H___

#include "Dx9RenderSystemPrerequisites.hpp"

#include <DynamicTexture.hpp>

namespace Dx9Render
{
	class DxDynamicTexture
		:	public Castor3D::DynamicTexture
	{
	public:
		DxDynamicTexture( DxRenderSystem * p_pRenderSystem );
		virtual ~DxDynamicTexture();

		virtual bool Create();
		virtual void Destroy();
		virtual void Cleanup();
		virtual uint8_t * Lock( uint32_t p_uiLock );
		virtual void Unlock( bool p_bModified );
		virtual void GenerateMipmaps() {}
		virtual void Fill( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat )
		{
			CASTOR_ASSERT( false );
		}

		inline IDirect3DTexture9 * GetDxTexture()const
		{
			return m_pTextureObject;
		}
		inline IDirect3DSurface9 * GetDxSurface()const
		{
			return m_pSurface;
		}

	private:
		virtual bool DoInitialise();
		virtual bool DoBind();
		virtual void DoUnbind();

	private:
		IDirect3DTexture9 * m_pTextureObject;
		IDirect3DSurface9 * m_pSurface;
		IDirect3DSurface9 * m_pLockSurface;
		DxRenderSystem * m_pRenderSystem;
	};
}

#endif
