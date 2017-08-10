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
#ifndef ___TRS_FRAME_BUFFER_H___
#define ___TRS_FRAME_BUFFER_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>

namespace TestRender
{
	class TestFrameBuffer
		: public castor3d::FrameBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_gl		The OpenGL APIs.
		 *\para[in]		engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_gl		Les APIs OpenGL.
		 *\para[in]		engine	Le moteur.
		 */
		TestFrameBuffer( castor3d::Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~TestFrameBuffer();
		/**
		 *\copydoc		castor3d::FrameBuffer::create
		 */
		bool create()override;
		/**
		 *\copydoc		castor3d::FrameBuffer::Destroy
		 */
		void destroy()override;
		/**
		 *\copydoc		castor3d::FrameBuffer::setDrawBuffers
		 */
		void setDrawBuffers( AttachArray const & p_attaches )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::setReadBuffer
		 */
		void setReadBuffer( castor3d::AttachmentPoint p_eAttach, uint8_t p_index )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::IsComplete
		 */
		bool isComplete()const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::downloadBuffer
		 */
		void downloadBuffer( castor3d::AttachmentPoint p_point, uint8_t p_index, castor::PxBufferBaseSPtr p_buffer )override;
		/**
		 *\copydoc		castor3d::FrameBuffer::CreateColourRenderBuffer
		 */
		castor3d::ColourRenderBufferSPtr createColourRenderBuffer( castor::PixelFormat p_format )override;
		/**
		 *\copydoc		castor3d::FrameBuffer::CreateDepthStencilRenderBuffer
		 */
		castor3d::DepthStencilRenderBufferSPtr createDepthStencilRenderBuffer( castor::PixelFormat p_format )override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::RenderBufferAttachmentSPtr createAttachment( castor3d::RenderBufferSPtr p_renderBuffer )override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::TextureAttachmentSPtr createAttachment( castor3d::TextureLayoutSPtr p_texture )override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::TextureAttachmentSPtr createAttachment( castor3d::TextureLayoutSPtr p_texture, castor3d::CubeMapFace p_face )override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createAttachment
		 */
		castor3d::TextureAttachmentSPtr createAttachment( castor3d::TextureLayoutSPtr p_texture, castor3d::CubeMapFace p_face, uint32_t p_mipLevel )override;

	private:
		/**
		 *\copydoc		castor3d::FrameBuffer::doBind
		 */
		void doBind( castor3d::FrameBufferTarget p_target )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doUnbind
		 */
		void doUnbind()const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doBlitInto
		 */
		void doBlitInto( castor3d::FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, castor::FlagCombination< castor3d::BufferComponent > const & p_components )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doStretchInto
		 */
		void doStretchInto( castor3d::FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, castor::FlagCombination< castor3d::BufferComponent > const & p_components, castor3d::InterpolationMode p_interpolation )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doClear
		 */
		void doClear( castor3d::BufferComponents p_targets )override;
	};
}

#endif
