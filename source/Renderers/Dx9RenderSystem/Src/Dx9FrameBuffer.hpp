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
#ifndef ___DX9_FRAME_BUFFER_H___
#define ___DX9_FRAME_BUFFER_H___

#include "Dx9RenderSystemPrerequisites.hpp"

#include <FrameBuffer.hpp>

#if defined( _WIN32 )
namespace Dx9Render
{
	class DxFrameBuffer
		: public Castor3D::FrameBuffer
	{
	private:
		typedef std::pair< IDirect3DSurface9 *, IDirect3DSurface9 * > SrcDstPair;
		DECLARE_VECTOR( SrcDstPair, SrcDstPair );
		DECLARE_MAP( DxFrameBufferSPtr, SrcDstPairArray, BlitScheme );
		DECLARE_MAP( DWORD, Castor3D::DynamicTextureSPtr, TextureIdx );
		DECLARE_MAP( DWORD, Castor3D::RenderBufferSPtr, RenderBufferIdx );
		DECLARE_ARRAY( IDirect3DSurface9 *, 15, D3DSurface );

	public:
		DxFrameBuffer( DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameBuffer();

		virtual bool Create( int p_iSamplesCount );
		virtual void Destroy();
		virtual bool SetDrawBuffers( uint32_t p_uiSize, Castor3D::eATTACHMENT_POINT const * p_eAttaches );
		virtual bool SetDrawBuffers();
		virtual bool SetReadBuffer( Castor3D::eATTACHMENT_POINT )
		{
			return true;
		}
		virtual bool IsComplete()
		{
			return true;
		}

		IDirect3DSurface9 * GetSurface( Castor3D::eATTACHMENT_POINT p_eAttachmentPoint );

		virtual Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );
		virtual Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );

	private:
		virtual bool DoBind( Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		virtual void DoUnbind();
		virtual void DoAttach( Castor3D::TextureAttachmentRPtr p_pAttach );
		virtual void DoDetach( Castor3D::TextureAttachmentRPtr p_pAttach );
		virtual void DoAttach( Castor3D::RenderBufferAttachmentRPtr p_pAttach );
		virtual void DoDetach( Castor3D::RenderBufferAttachmentRPtr p_pAttach );
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::DynamicTextureSPtr p_pTexture, Castor3D::eTEXTURE_TARGET p_eTarget, int p_iLayer = 0 );
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::RenderBufferSPtr p_pRenderBuffer );
		virtual void DoDetachAll();
		virtual bool DoStretchInto( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, Castor3D::eINTERPOLATION_MODE p_eInterpolation );

	private:
		IDirect3DDevice9 * m_pDevice;
		DxRenderSystem * m_pRenderSystem;
		bool m_bModified;
		BlitSchemeMap m_mapBlitSchemes;
		TextureIdxMap m_mapAttTex;
		RenderBufferIdxMap m_mapAttRbo;
		D3DSurfaceArray m_arrayOldSurfaces;
		IDirect3DSurface9 * m_pOldDepthStencilSurface;
	};
}
#endif

#endif
