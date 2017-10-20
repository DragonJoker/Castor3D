/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_BACK_BUFFERS_H___
#define ___C3DGLES3_BACK_BUFFERS_H___

#include "GlES3RenderSystemPrerequisites.hpp"

#include "Common/GlES3Holder.hpp"

#include <FrameBuffer/BackBuffers.hpp>

namespace GlES3Render
{
	class GlES3BackBuffers
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
		GlES3BackBuffers( OpenGlES3 & p_gl, Castor3D::Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3BackBuffers();
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
		void DownloadBuffer( Castor3D::AttachmentPoint p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer )override;
		/**
		 *\copydoc		Castor3D::BackBuffers::Bind
		 */
		void Bind( Castor3D::WindowBuffer p_buffer, Castor3D::FrameBufferTarget p_target )const override;
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
		void DoBlitInto( Castor3D::FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, Castor::FlagCombination< Castor3D::BufferComponent > const & p_components )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoStretchInto
		 */
		void DoStretchInto( Castor3D::FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, Castor::FlagCombination< Castor3D::BufferComponent > const & p_components, Castor3D::InterpolationMode p_interpolation )const override;

	private:
		GlES3FrameBufferMode m_glBindingMode;
	};
}

#endif
