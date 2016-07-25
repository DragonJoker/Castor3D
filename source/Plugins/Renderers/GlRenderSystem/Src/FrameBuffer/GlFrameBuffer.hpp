/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GL_FRAME_BUFFER_H___
#define ___GL_FRAME_BUFFER_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "Common/GlBindable.hpp"

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>

namespace GlRender
{
	class GlFrameBuffer
		: public Castor3D::FrameBuffer
		, private Bindable< std::function< bool( int, uint32_t * ) >,
							std::function< bool( int, uint32_t const * ) >,
							std::function< bool( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< bool( int, uint32_t * ) >,
									   std::function< bool( int, uint32_t const * ) >,
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
		virtual void SetDrawBuffers( AttachArray const & p_attaches );
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetReadBuffer
		 */
		virtual void SetReadBuffer( Castor3D::eATTACHMENT_POINT p_eAttach, uint8_t p_index );
		/**
		 *\copydoc		Castor3D::FrameBuffer::IsComplete
		 */
		virtual bool IsComplete()const;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DownloadBuffer
		 */
		virtual bool DownloadBuffer( Castor3D::eATTACHMENT_POINT p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateColourRenderBuffer
		 */
		virtual Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_format );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateDepthStencilRenderBuffer
		 */
		virtual Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_format );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateAttachment
		 */
		virtual Castor3D::RenderBufferAttachmentSPtr CreateAttachment( Castor3D::RenderBufferSPtr p_renderBuffer );
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateAttachment
		 */
		virtual Castor3D::TextureAttachmentSPtr CreateAttachment( Castor3D::TextureLayoutSPtr p_texture );

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
		virtual bool DoBlitInto( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoStretchInto
		 */
		virtual bool DoStretchInto( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, Castor3D::InterpolationMode p_interpolation );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoClear
		 */
		virtual void DoClear( uint32_t p_targets );

	private:
		eGL_FRAMEBUFFER_MODE m_bindingMode;
	};
}

#endif
