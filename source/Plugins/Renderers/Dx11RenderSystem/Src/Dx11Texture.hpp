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
#ifndef ___DX11_TEXTURE_H___
#define ___DX11_TEXTURE_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <DynamicTexture.hpp>

namespace Dx11Render
{
	class DxTexture
	{
	public:
		DxTexture( DxRenderSystem & p_renderSystem, bool p_static );
		virtual ~DxTexture();

		virtual bool Initialise( Castor3D::eTEXTURE_DIMENSION p_dimension, Castor::PxBufferBaseSPtr p_pixelBuffer, uint32_t p_depth, bool p_renderTarget, uint32_t p_samples );
		virtual void Cleanup();
		virtual bool Bind( uint32_t p_index );
		virtual void Unbind( uint32_t p_index );
		virtual uint8_t * Lock( uint32_t p_lock );
		virtual void Unlock( bool p_modified );

		inline ID3D11RenderTargetView * GetRenderTargetView()const
		{
			return m_renderTargetView;
		}

		inline ID3D11ShaderResourceView * GetShaderResourceView()const
		{
			return m_shaderResourceView;
		}

		inline Castor::PxBufferBaseSPtr GetPixelBuffer()const
		{
			return m_pixelBuffer;
		}

	private:

		bool DoInitialiseBuffer();
		bool DoInitialise1D();
		bool DoInitialise1DArray();
		bool DoInitialise2D();
		bool DoInitialise2DArray();
		bool DoInitialise2DMS();
		bool DoInitialise2DMSArray();
		bool DoInitialise3D();
		bool DoInitialiseCube();
		bool DoInitialiseCubeArray();
		void DoInitTex1DDesc( D3D11_TEXTURE1D_DESC & p_desc );
		void DoInitTex1DData( D3D11_SUBRESOURCE_DATA & p_data );
		void DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_desc );
		void DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_data );
		void DoInitTex3DDesc( D3D11_TEXTURE3D_DESC & p_desc );
		void DoInitTex3DData( D3D11_SUBRESOURCE_DATA & p_data );
		bool DoCreateRenderTargetView( D3D11_RTV_DIMENSION p_dimension, ID3D11Resource * p_texture );
		bool DoCreateShaderResourceView( D3D11_SRV_DIMENSION p_dimension, ID3D11Resource * p_texture );
		DXGI_FORMAT DoCheckPixelFormat( DXGI_FORMAT p_format );

	private:
		Castor::PxBufferBaseSPtr m_pixelBuffer;
		DxRenderSystem * m_renderSystem;
		ID3D11RenderTargetView * m_renderTargetView;
		ID3D11ShaderResourceView * m_shaderResourceView;
		uint32_t m_depth;
		bool m_renderTarget;
		bool m_static;
		uint32_t m_samples;
	};
}

#endif
