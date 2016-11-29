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
#ifndef ___GL_BACK_BUFFERS_H___
#define ___GL_BACK_BUFFERS_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "Common/GlHolder.hpp"

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
		bool Create()override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::Destroy
		 */
		void Destroy()override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetDrawBuffers
		 */
		void SetDrawBuffers( Castor3D::FrameBuffer::AttachArray const & p_attaches )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DownloadBuffer
		 */
		bool DownloadBuffer( Castor3D::AttachmentPoint p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer )override;
		/**
		 *\copydoc		Castor3D::BackBuffers::Bind
		 */
		bool Bind( Castor3D::WindowBuffer p_buffer, Castor3D::FrameBufferTarget p_target )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetReadBuffer
		 */
		void SetReadBuffer( Castor3D::AttachmentPoint, uint8_t )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateColourRenderBuffer
		 */
		Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::PixelFormat p_format )override
		{
			return nullptr;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateDepthStencilRenderBuffer
		 */
		Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::PixelFormat p_format )override
		{
			return nullptr;
		}
		/**
		 *\copydoc		Castor3D::FrameBuffer::IsComplete
		 */
		bool IsComplete()const override
		{
			return true;
		}

	private:
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoClear
		 */
		void DoClear( uint32_t p_targets )override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBlitInto
		 */
		bool DoBlitInto( Castor3D::FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, Castor::FlagCombination< Castor3D::BufferComponent > const & p_components )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoStretchInto
		 */
		bool DoStretchInto( Castor3D::FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, Castor::FlagCombination< Castor3D::BufferComponent > const & p_components, Castor3D::InterpolationMode p_interpolation )const override;

	private:
		GlFrameBufferMode m_glBindingMode;
	};
}

#endif
