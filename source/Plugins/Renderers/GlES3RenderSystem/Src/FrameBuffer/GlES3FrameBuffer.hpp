/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_FRAME_BUFFER_H___
#define ___C3DGLES3_FRAME_BUFFER_H___

#include "GlES3RenderSystemPrerequisites.hpp"

#include "Common/GlES3Bindable.hpp"

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>

namespace GlES3Render
{
	class GlES3FrameBuffer
		: public Castor3D::FrameBuffer
		, private Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >;

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
		GlES3FrameBuffer( OpenGlES3 & p_gl, Castor3D::Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3FrameBuffer();
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
		void SetDrawBuffers( AttachArray const & p_attaches )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::SetReadBuffer
		 */
		void SetReadBuffer( Castor3D::AttachmentPoint p_eAttach, uint8_t p_index )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::IsComplete
		 */
		bool IsComplete()const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DownloadBuffer
		 */
		void DownloadBuffer( Castor3D::AttachmentPoint p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer )override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateColourRenderBuffer
		 */
		Castor3D::ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::PixelFormat p_format )override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateDepthStencilRenderBuffer
		 */
		Castor3D::DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::PixelFormat p_format )override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateAttachment
		 */
		Castor3D::RenderBufferAttachmentSPtr CreateAttachment( Castor3D::RenderBufferSPtr p_renderBuffer )override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateAttachment
		 */
		Castor3D::TextureAttachmentSPtr CreateAttachment( Castor3D::TextureLayoutSPtr p_texture )override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::CreateAttachment
		 */
		Castor3D::TextureAttachmentSPtr CreateAttachment( Castor3D::TextureLayoutSPtr p_texture, Castor3D::CubeMapFace p_face )override;

	public:
		using BindableType::GetGlES3Name;
		using BindableType::GetOpenGlES3;
		using BindableType::IsValid;

	private:
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBind
		 */
		void DoBind( Castor3D::FrameBufferTarget p_target )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoUnbind
		 */
		void DoUnbind()const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBlitInto
		 */
		void DoBlitInto(
			Castor3D::FrameBuffer const & p_buffer,
			Castor::Rectangle const & p_rect,
			Castor::FlagCombination< Castor3D::BufferComponent > const & p_components )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoStretchInto
		 */
		void DoStretchInto(
			Castor3D::FrameBuffer const & p_buffer,
			Castor::Rectangle const & p_rectSrc,
			Castor::Rectangle const & p_rectDst,
			Castor::FlagCombination< Castor3D::BufferComponent > const & p_components,
			Castor3D::InterpolationMode p_interpolation )const override;
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoClear
		 */
		void DoClear( uint32_t p_targets )override;

	private:
		mutable GlES3FrameBufferMode m_bindingMode;
	};
}

#endif
