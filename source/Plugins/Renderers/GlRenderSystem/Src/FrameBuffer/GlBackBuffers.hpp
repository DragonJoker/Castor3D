/*
See LICENSE file in root folder
*/
#ifndef ___GL_BACK_BUFFERS_H___
#define ___GL_BACK_BUFFERS_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "Common/GlHolder.hpp"

#include <FrameBuffer/BackBuffers.hpp>

namespace GlRender
{
	class GlBackBuffers
		: public castor3d::BackBuffers
		, public Holder
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
		GlBackBuffers( OpenGl & p_gl, castor3d::Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlBackBuffers();
		/**
		 *\copydoc		castor3d::FrameBuffer::initialise
		 */
		bool initialise()override;
		/**
		 *\copydoc		castor3d::FrameBuffer::cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::FrameBuffer::setDrawBuffers
		 */
		void setDrawBuffers( castor3d::FrameBuffer::AttachArray const & p_attaches )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::downloadBuffer
		 */
		void downloadBuffer( castor3d::AttachmentPoint p_point, uint8_t p_index, castor::PxBufferBaseSPtr p_buffer )override;
		/**
		 *\copydoc		castor3d::BackBuffers::bind
		 */
		void bind( castor3d::WindowBuffer p_buffer, castor3d::FrameBufferTarget p_target )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::setReadBuffer
		 */
		void setReadBuffer( castor3d::AttachmentPoint, uint8_t )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::createColourRenderBuffer
		 */
		castor3d::ColourRenderBufferSPtr createColourRenderBuffer( castor::PixelFormat p_format )override
		{
			return nullptr;
		}
		/**
		 *\copydoc		castor3d::FrameBuffer::createDepthStencilRenderBuffer
		 */
		castor3d::DepthStencilRenderBufferSPtr createDepthStencilRenderBuffer( castor::PixelFormat p_format )override
		{
			return nullptr;
		}
		/**
		 *\copydoc		castor3d::FrameBuffer::isComplete
		 */
		bool isComplete()const override
		{
			return true;
		}

	private:
		/**
		 *\copydoc		castor3d::FrameBuffer::doClear
		 */
		void doClear( castor3d::BufferComponents p_targets )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doBlitInto
		 */
		void doBlitInto( castor3d::FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, castor::FlagCombination< castor3d::BufferComponent > const & p_components )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doStretchInto
		 */
		void doStretchInto( castor3d::FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, castor::FlagCombination< castor3d::BufferComponent > const & p_components, castor3d::InterpolationMode p_interpolation )const override;

	private:
		GlFrameBufferMode m_glBindingMode;
	};
}

#endif
