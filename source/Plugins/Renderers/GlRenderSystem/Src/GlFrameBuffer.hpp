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
#ifndef ___GL_FRAME_BUFFER_H___
#define ___GL_FRAME_BUFFER_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "GlBindable.hpp"

#include <FrameBuffer.hpp>
#include <TextureAttachment.hpp>
#include <RenderBufferAttachment.hpp>

namespace GlRender
{
	class GlFrameBuffer
		: public Castor3D::FrameBuffer
		, public Castor::NonCopyable
		, private Bindable< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) >,
						   std::function< bool( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) >,
			std::function< bool( uint32_t ) > >;

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
		GlFrameBuffer( OpenGl & p_gl, Castor3D::Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlFrameBuffer();
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual bool Create( int p_iSamplesCount );
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual void Destroy();
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual void SetDrawBuffers( AttachArray const & p_attaches );
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual void SetReadBuffer( Castor3D::eATTACHMENT_POINT p_eAttach, uint8_t p_index );
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual bool IsComplete()const;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DownloadBuffer
		 */
		virtual bool DownloadBuffer( Castor3D::eATTACHMENT_POINT p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		virtual Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat );

	public:
		using BindableType::GetGlName;
		using BindableType::GetOpenGl;
		using BindableType::IsValid;

	private:
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBind
		 */
		virtual bool DoBind( Castor3D::eFRAMEBUFFER_TARGET p_target );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoUnbind
		 */
		virtual void DoUnbind();
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBlitInto
		 */
		virtual bool DoBlitInto( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, Castor3D::eINTERPOLATION_MODE p_eInterpolationMode );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoUpdateClearColour
		 */
		virtual void DoUpdateClearColour();
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoClear
		 */
		virtual void DoClear( uint32_t p_targets );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoInitialise
		 */
		virtual bool DoInitialise( Castor::Size const & p_size )
		{
			return true;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoCleanup
		 */
		virtual void DoCleanup()
		{
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoResize
		 */
		virtual void DoResize( Castor::Size const & p_size )
		{
		}

	private:
		eGL_FRAMEBUFFER_MODE m_bindingMode;
	};
}

#endif
