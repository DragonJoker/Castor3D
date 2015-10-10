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
#ifndef ___DX11_FRAME_BUFFER_H___
#define ___DX11_FRAME_BUFFER_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <FrameBuffer.hpp>
#include <TextureAttachment.hpp>
#include <RenderBufferAttachment.hpp>

#if defined( _WIN32 )
namespace Dx11Render
{
	class DxFrameBuffer
		:	public Castor3D::FrameBuffer
	{
	private:
		typedef std::pair< ID3D11View *, ID3D11View * > SrcDstPair;
		DECLARE_VECTOR( SrcDstPair, 										SrcDstPair	);
		DECLARE_MAP( Castor3D::eATTACHMENT_POINT,	DxDynamicTextureSPtr,	DxTexturePtrAttach	);
		DECLARE_MAP( Castor3D::eATTACHMENT_POINT,	DxRenderBufferSPtr,		DxBufferPtrAttach	);
		DECLARE_MAP( DxFrameBufferSPtr,				SrcDstPairArray,		BlitScheme	);
		DECLARE_VECTOR( ID3D11RenderTargetView *,							D3D11RenderTargetView	);

	public:
		DxFrameBuffer( DxRenderSystem * p_renderSystem );
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

		virtual Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );
		virtual Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );

		ID3D11View * GetSurface( Castor3D::eATTACHMENT_POINT );

	private:
		virtual bool DoBind( Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		virtual void DoUnbind();
		virtual void DoAttach( Castor3D::TextureAttachmentRPtr p_pAttach )
		{
			return DoAttachFba( p_pAttach );
		}
		virtual void DoDetach( Castor3D::TextureAttachmentRPtr p_pAttach )
		{
			return DoDetachFba( p_pAttach );
		}
		virtual void DoAttach( Castor3D::RenderBufferAttachmentRPtr p_pAttach )
		{
			return DoAttachFba( p_pAttach );
		}
		virtual void DoDetach( Castor3D::RenderBufferAttachmentRPtr p_pAttach )
		{
			return DoDetachFba( p_pAttach );
		}
		virtual void DoAttachFba( Castor3D::FrameBufferAttachmentRPtr p_pAttach );
		virtual void DoDetachFba( Castor3D::FrameBufferAttachmentRPtr p_pAttach );
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::DynamicTextureSPtr p_pTexture, Castor3D::eTEXTURE_TARGET p_eTarget, int p_iLayer = 0 );
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::RenderBufferSPtr p_pRenderBuffer );
		virtual void DoDetachAll();
		virtual bool DoStretchInto( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, Castor3D::eINTERPOLATION_MODE p_eInterpolation );
		void DoCleanupOld();

	private:
		DxRenderSystem * m_renderSystem;
		D3D11RenderTargetViewArray m_arrayOldRenderTargets;
		ID3D11DepthStencilView * m_pOldDepthStencilView;
	};
}
#endif

#endif
