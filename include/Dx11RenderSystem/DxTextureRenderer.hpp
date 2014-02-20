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
#ifndef ___Dx11_TextureRenderer___
#define ___Dx11_TextureRenderer___

#include "Module_DxRender.hpp"
#include <Castor3D/Sampler.hpp>

namespace Dx11Render
{
	class DxSamplerRenderer : public Castor3D::SamplerRenderer
	{
	private:
		DxRenderSystem *		m_pDxRenderSystem;
		D3D11_SAMPLER_DESC		m_tex2dSampler;
		ID3D11SamplerState *	m_pSamplerState;

	public:
		DxSamplerRenderer( DxRenderSystem * p_pRenderSystem );
		virtual ~DxSamplerRenderer();
		
		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Bind( Castor3D::eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex );
		virtual void Unbind();
	};

	class DxStaticTexture : public Castor3D::StaticTexture
	{
	private:
		ID3D11ShaderResourceView *	m_pShaderResourceView;
		DxRenderSystem *			m_pRenderSystem;

	public:
		DxStaticTexture( DxRenderSystem * p_pRenderSystem );
		virtual ~DxStaticTexture();

		virtual bool		Create			();
		virtual void		Destroy			();
		virtual void		Cleanup			();
		virtual uint8_t *	Lock			( uint32_t p_uiLock );
		virtual void		Unlock			( bool p_bModified );
		virtual void		GenerateMipmaps	(){}
		
		inline ID3D11ShaderResourceView * GetShaderResourceView()const { return m_pShaderResourceView; }

	private:
		virtual bool DoInitialise	();
		virtual bool DoBind			();
		virtual void DoUnbind		();

		void DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_tex2dDesc );
		void DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_tex2dData );
	};

	class DxDynamicTexture : public Castor3D::DynamicTexture
	{
	private:
		DxRenderSystem *			m_pRenderSystem;
		ID3D11RenderTargetView *	m_pRenderTargetView;
		ID3D11ShaderResourceView *	m_pShaderResourceView;

	public:
		DxDynamicTexture( DxRenderSystem * p_pRenderSystem );
		virtual ~DxDynamicTexture();

		virtual bool		Create			();
		virtual void		Destroy			();
		virtual void		Cleanup			();
		virtual uint8_t *	Lock			( uint32_t p_uiLock );
		virtual void		Unlock			( bool p_bModified );
		virtual void		GenerateMipmaps	(){}
		virtual void		Fill			( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat ) { CASTOR_ASSERT( false ); }

		inline ID3D11RenderTargetView * GetRenderTargetView()const { return m_pRenderTargetView; }
		inline ID3D11ShaderResourceView * GetShaderResourceView()const { return m_pShaderResourceView; }

	private:
		virtual bool DoInitialise	();
		virtual bool DoBind			();
		virtual void DoUnbind		();

		void DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_tex2dDesc );
		void DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_tex2dData );
	};

	class DxTextureRenderer : public Castor3D::TextureRenderer
	{
	public:
		DxTextureRenderer( DxRenderSystem * p_pRenderSystem );
		virtual ~DxTextureRenderer();

		virtual bool Render();
		virtual void EndRender();
	};
}

#endif