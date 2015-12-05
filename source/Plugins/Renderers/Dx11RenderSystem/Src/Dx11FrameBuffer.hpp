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
		: public Castor3D::FrameBuffer
	{
	private:
		DECLARE_MAP( Castor3D::eATTACHMENT_POINT, DxDynamicTextureSPtr, DxTexturePtrAttach );
		DECLARE_MAP( Castor3D::eATTACHMENT_POINT, DxRenderBufferSPtr, DxBufferPtrAttach );
		DECLARE_VECTOR( ID3D11RenderTargetView *, D3D11RenderTargetView );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_renderSystem	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_renderSystem	Le moteur.
		 */
		DxFrameBuffer( DxRenderSystem * p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~DxFrameBuffer();
		/**
		 *\copydoc		Castor3D::FrameBuffer::Create
		 */
		virtual bool Create( int p_iSamplesCount );
		/**
		 *\copydoc		Castor3D::FrameBuffer::Destroy
		 */
		virtual void Destroy();
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual void SetDrawBuffers( AttachArray const & p_attaches );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateColourRenderBuffer
		 */
		virtual Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateDepthStencilRenderBuffer
		 */
		virtual Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DownloadBuffer
		 */
		virtual bool DownloadBuffer( Castor3D::eATTACHMENT_POINT p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetReadBuffer
		 */
		virtual void SetReadBuffer( Castor3D::eATTACHMENT_POINT p_point, uint8_t p_index );
		/**
		 *\copydoc		Castor3D::FrameBuffer::IsComplete
		 */
		virtual bool IsComplete()const
		{
			return true;
		}

		ID3D11View * GetSurface( Castor3D::eATTACHMENT_POINT );

	private:
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoInitialise
		 */
		virtual bool DoInitialise( Castor::Size const & p_size );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoCleanup
		 */
		virtual void DoCleanup();
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoUpdateClearColour
		 */
		virtual void DoUpdateClearColour();
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoClear
		 */
		virtual void DoClear( uint32_t p_targets );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBind
		 */
		virtual bool DoBind( Castor3D::eFRAMEBUFFER_TARGET p_target );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoUnbind
		 */
		virtual void DoUnbind();
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoResize
		 */
		virtual void DoResize( Castor::Size const & p_size );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBlitInto
		 */
		virtual bool DoBlitInto( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, Castor3D::eINTERPOLATION_MODE p_interpolation );

	private:
		DxRenderSystem * m_renderSystem;
		D3D11RenderTargetViewArray m_arrayOldRenderTargets;
		ID3D11DepthStencilView * m_pOldDepthStencilView;
		//!\~english The texture used to retrieve color buffers.	\~french La texture utilisée pour récupérer les tampons de couleur.
		DxDynamicTextureSPtr m_colorBuffer;
		//!\~english The texture used to retrieve depth buffers.	\~french La texture utilisée pour récupérer les tampons de profondeur.
		DxDynamicTextureSPtr m_depthBuffer;
		FLOAT m_fClearColour[4];
	};
}
#endif

#endif
