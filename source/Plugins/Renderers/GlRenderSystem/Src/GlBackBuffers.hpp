/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___GL_BACK_BUFFERS_H___
#define ___GL_BACK_BUFFERS_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "GlHolder.hpp"

#include <FrameBuffer/BackBuffers.hpp>

namespace GlRender
{
	class GlBackBuffers
		: public Castor3D::BackBuffers
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_gl		The OpenGL APIs.
		 *\para[in]		p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_gl		Les APIs OpenGL.
		 *\para[in]		p_engine	Le moteur.
		 */
		GlBackBuffers( OpenGl & p_gl, Castor3D::Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlBackBuffers();
		/**
		 *\copydoc		Castor3D::FrameBuffer::Create
		 */
		virtual bool Create();
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
		 *\copydoc		Castor3D::FrameBuffer::SetReadBuffer
		 */
		virtual void SetReadBuffer( Castor3D::eATTACHMENT_POINT, uint8_t );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateColourRenderBuffer
		 */
		virtual Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_format )
		{
			return nullptr;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateDepthStencilRenderBuffer
		 */
		virtual Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_format )
		{
			return nullptr;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::IsComplete
		 */
		virtual bool IsComplete()const
		{
			return true;
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
		virtual bool DoBlitInto( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoStretchInto
		 */
		virtual bool DoStretchInto( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, Castor3D::InterpolationMode p_interpolation );

	private:
		eGL_FRAMEBUFFER_MODE m_glBindingMode;
	};
}

#endif
