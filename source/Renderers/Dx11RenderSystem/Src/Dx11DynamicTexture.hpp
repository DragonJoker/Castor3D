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
#ifndef ___DX11_DYNAMIC_TEXTURE_H___
#define ___DX11_DYNAMIC_TEXTURE_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <DynamicTexture.hpp>

namespace Dx11Render
{
	class DxDynamicTexture
		:	public Castor3D::DynamicTexture
	{
	public:
		DxDynamicTexture( DxRenderSystem * p_renderSystem );
		virtual ~DxDynamicTexture();

		virtual bool Create();
		virtual void Destroy();
		virtual void Cleanup();
		virtual uint8_t * Lock( uint32_t p_lock );
		virtual void Unlock( bool p_modified );
		virtual void GenerateMipmaps() {}
		virtual void Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
		{
			CASTOR_ASSERT( false );
		}

		inline ID3D11RenderTargetView * GetRenderTargetView()const
		{
			return m_renderTargetView;
		}
		inline ID3D11ShaderResourceView * GetShaderResourceView()const
		{
			return m_shaderResourceView;
		}

	private:
		virtual bool DoInitialise();
		virtual bool DoBind( uint32_t p_index );
		virtual void DoUnbind( uint32_t p_index );

		void DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_desc );
		void DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_data );

	private:
		DxRenderSystem * m_renderSystem;
		ID3D11RenderTargetView * m_renderTargetView;
		ID3D11ShaderResourceView * m_shaderResourceView;
	};
}

#endif
