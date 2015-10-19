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

		virtual bool Initialise( Castor3D::eTEXTURE_TYPE p_dimension, Castor::PxBufferBaseSPtr p_pixelBuffer, uint32_t p_depth, uint8_t p_cpuAccess, uint8_t p_gpuAccess, bool p_renderTarget, uint32_t p_samples );
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

		inline ID3D11Texture1D * GetTexture1D()const
		{
			REQUIRE( m_type == Castor3D::eTEXTURE_TYPE_1D
					 || m_type == Castor3D::eTEXTURE_TYPE_1DARRAY );
			return m_texture.m_texture1D;
		}

		inline ID3D11Texture2D * GetTexture2D()const
		{
			REQUIRE( m_type == Castor3D::eTEXTURE_TYPE_2D
					 || m_type == Castor3D::eTEXTURE_TYPE_2DARRAY
					 || m_type == Castor3D::eTEXTURE_TYPE_2DMS
					 || m_type == Castor3D::eTEXTURE_TYPE_2DMSARRAY );
			return m_texture.m_texture2D;
		}

		inline ID3D11Texture3D * GetTexture3D()const
		{
			REQUIRE( m_type == Castor3D::eTEXTURE_TYPE_3D );
			return m_texture.m_texture3D;
		}

	private:

		bool DoInitialiseBuffer( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialise1D( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialise1DArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialise2D( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialise2DArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialise2DMS( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialise2DMSArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialise3D( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialiseCube( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		bool DoInitialiseCubeArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		D3D11_TEXTURE1D_DESC DoInitTex1DDesc( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		D3D11_SUBRESOURCE_DATA DoInitTex1DData();
		D3D11_TEXTURE2D_DESC DoInitTex2DDesc( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		D3D11_SUBRESOURCE_DATA DoInitTex2DData();
		D3D11_TEXTURE3D_DESC DoInitTex3DDesc( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		D3D11_SUBRESOURCE_DATA DoInitTex3DData();
		bool DoCreateRenderTargetView( D3D11_RTV_DIMENSION p_dimension, ID3D11Resource * p_texture );
		bool DoCreateShaderResourceView( D3D11_SRV_DIMENSION p_dimension, ID3D11Resource * p_texture );
		DXGI_FORMAT DoCheckPixelFormat( DXGI_FORMAT p_format );

		template< typename TextureType >
		void DoCreateResources( TextureType * p_texture, bool p_sr, D3D11_SRV_DIMENSION p_srv, bool p_rt, D3D11_RTV_DIMENSION p_rtv )
		{
			if ( p_sr )
			{
				DoCreateShaderResourceView( p_srv, p_texture );
			}

			if ( p_rt )
			{
				DoCreateRenderTargetView( p_rtv, p_texture );
			}
		}

	private:
		Castor::PxBufferBaseSPtr m_pixelBuffer;
		DxRenderSystem * m_renderSystem;
		ID3D11RenderTargetView * m_renderTargetView;
		ID3D11ShaderResourceView * m_shaderResourceView;
		uint32_t m_depth;
		bool m_renderTarget;
		bool m_static;
		uint32_t m_samples;
		Castor3D::eTEXTURE_TYPE m_type;
		union
		{
			ID3D11Texture1D * m_texture1D;
			ID3D11Texture2D * m_texture2D;
			ID3D11Texture3D * m_texture3D;
		} m_texture;
	};
}

#endif
