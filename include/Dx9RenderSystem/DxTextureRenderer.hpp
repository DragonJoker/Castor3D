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
#ifndef ___Dx9_TextureRenderer___
#define ___Dx9_TextureRenderer___

#include "Module_Dx9Render.hpp"
#include <Castor3D/Sampler.hpp>

namespace Dx9Render
{
	class DxSamplerRenderer : public Castor3D::SamplerRenderer
	{
	private:
		DxRenderSystem *	m_pDxRenderSystem;
		static Castor3D::SamplerSPtr sm_pCurrent;

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
		IDirect3DTexture9 * m_pTextureObject;
		DxRenderSystem *	m_pRenderSystem;

	public:
		DxStaticTexture( DxRenderSystem * p_pRenderSystem );
		virtual ~DxStaticTexture();

		virtual bool		Create			();
		virtual void		Destroy			();
		virtual void		Cleanup			();
		virtual uint8_t *	Lock			( uint32_t p_uiLock );
		virtual void		Unlock			( bool p_bModified );
		virtual void		GenerateMipmaps	(){}
		
		inline IDirect3DTexture9 * GetDxTexture()const { return m_pTextureObject; }

	private:
		virtual bool DoInitialise	();
		virtual bool DoBind			();
		virtual void DoUnbind		();
	};

	class DxDynamicTexture : public Castor3D::DynamicTexture
	{
	private:
		IDirect3DTexture9 * m_pTextureObject;
		IDirect3DSurface9 * m_pSurface;
		IDirect3DSurface9 * m_pLockSurface;
		DxRenderSystem *	m_pRenderSystem;

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
		
		inline IDirect3DTexture9 * GetDxTexture()const { return m_pTextureObject; }
		inline IDirect3DSurface9 * GetDxSurface()const { return m_pSurface; }

	private:
		virtual bool DoInitialise	();
		virtual bool DoBind			();
		virtual void DoUnbind		();
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