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
#ifndef ___DX11_BACK_BUFFERS_H___
#define ___DX11_BACK_BUFFERS_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <BackBuffers.hpp>

#if defined( _WIN32 )
namespace Dx11Render
{
	class DxBackBuffers
		: public Castor3D::BackBuffers
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_renderSystem	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_renderSystem	Le moteur.
		 */
		DxBackBuffers( DxRenderSystem * p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~DxBackBuffers();
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
		virtual void SetDrawBuffers( Castor3D::FrameBuffer::AttachArray const & p_attaches );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DownloadBuffer
		 */
		virtual bool DownloadBuffer( Castor3D::eATTACHMENT_POINT p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\copydoc		Castor3D::BackBuffers::Bind
		 */
		virtual bool Bind( Castor3D::eBUFFER p_buffer, Castor3D::eFRAMEBUFFER_TARGET p_target );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateColourRenderBuffer
		 */
		virtual Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat )
		{
			return nullptr;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateDepthStencilRenderBuffer
		 */
		virtual Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat )
		{
			return nullptr;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetReadBuffer
		 */
		virtual void SetReadBuffer( Castor3D::eATTACHMENT_POINT, uint8_t )
		{
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::IsComplete
		 */
		virtual bool IsComplete()const
		{
			return true;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::HasFixedSize
		 */
		virtual bool HasFixedSize()const
		{
			return true;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::GetSize
		 */
		virtual Castor::Size GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The color buffer.
		 *\~french
		 *\brief		Le tampon de couleurs.
		 */
		ID3D11RenderTargetView * GetRenderTargetView()const
		{
			return m_renderTargetView;
		}
		/**
		 *\~english
		 *\return		The depth buffer.
		 *\~french
		 *\brief		Le tampon de profondeur.
		 */
		ID3D11DepthStencilView * GetDepthStencilView()const
		{
			return m_depthStencilView;
		}

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
		 *\copydoc		Castor3D::FrameBuffer::DoResize
		 */
		virtual void DoResize( Castor::Size const & p_size );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBlitInto
		 */
		virtual bool DoBlitInto( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, Castor3D::eINTERPOLATION_MODE p_interpolation );

	private:
		DxRenderSystem * m_renderSystem;
		ID3D11RenderTargetView * m_renderTargetView;
		ID3D11DepthStencilView * m_depthStencilView;
		FLOAT m_fClearColour[4];
		//!\~english The texture used to retrieve color buffers.	\~french La texture utilisée pour récupérer les tampons de couleur.
		DxDynamicTextureSPtr m_colorBuffer;
		//!\~english The texture used to retrieve depth buffers.	\~french La texture utilisée pour récupérer les tampons de profondeur.
		DxDynamicTextureSPtr m_depthBuffer;
		//!\~english The back buffer fixed size.	\~french La taille fixe du tampon de fenêtre.
		Castor::Size m_size;
	};
}
#endif

#endif
