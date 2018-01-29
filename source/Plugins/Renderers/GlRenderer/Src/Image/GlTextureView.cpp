#include "Image/GlTextureView.hpp"

#include "Core/GlDevice.hpp"
#include "Image/GlTexture.hpp"
#include "Sync/GlImageMemoryBarrier.hpp"

namespace gl_renderer
{
	TextureView::TextureView( Device const & device
		, Texture const & texture
		, renderer::TextureType type
		, renderer::PixelFormat format
		, uint32_t baseMipLevel
		, uint32_t levelCount
		, uint32_t baseArrayLayer
		, uint32_t layerCount )
		: renderer::TextureView{ device, texture, type, format, baseMipLevel, levelCount, baseArrayLayer, layerCount }
		, m_device{ device }
		, m_target{ convert( type ) }
	{
		glLogCall( gl::GenTextures, 1, &m_texture );
		glLogCall( gl::TextureView
			, m_texture
			, m_target
			, texture.getImage()
			, getInternal( format )
			, baseMipLevel
			, levelCount
			, baseArrayLayer
			, layerCount );
	}

	TextureView::~TextureView()
	{
		glLogCall( gl::DeleteTextures, 1, &m_texture );
	}

	renderer::ImageMemoryBarrier TextureView::makeGeneralLayout( renderer::AccessFlags accessFlags )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eGeneral
			, ~( 0u )
			, accessFlags );
	}

	renderer::ImageMemoryBarrier TextureView::makeTransferDestination()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferDstOptimal
			, ~( 0u )
			, renderer::AccessFlag::eTransferWrite );
	}

	renderer::ImageMemoryBarrier TextureView::makeTransferSource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferSrcOptimal
			, ~( 0u )
			, renderer::AccessFlag::eShaderRead );
	}

	renderer::ImageMemoryBarrier TextureView::makeShaderInputResource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eShaderReadOnlyOptimal
			, ~( 0u )
			, renderer::AccessFlag::eTransferRead );
	}

	renderer::ImageMemoryBarrier TextureView::makeDepthStencilReadOnly()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilReadOnlyOptimal
			, ~( 0u )
			, renderer::AccessFlag::eShaderRead );
	}

	renderer::ImageMemoryBarrier TextureView::makeColourAttachment()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eColourAttachmentOptimal
			, ~( 0u )
			, renderer::AccessFlag::eColourAttachmentWrite );
	}

	renderer::ImageMemoryBarrier TextureView::makeDepthStencilAttachment()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilAttachmentOptimal
			, ~( 0u )
			, renderer::AccessFlag::eColourAttachmentWrite );
	}

	renderer::ImageMemoryBarrier TextureView::makePresentSource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::ePresentSrc
			, ~( 0u )
			, renderer::AccessFlag::eMemoryRead );
	}

	renderer::ImageMemoryBarrier TextureView::doMakeLayoutTransition( renderer::ImageLayout layout
		, uint32_t queueFamily
		, renderer::AccessFlags dstAccessMask )const
	{
		return renderer::ImageMemoryBarrier
		{
			0u,                                      // srcAccessMask
			dstAccessMask,                           // dstAccessMask
			renderer::ImageLayout::eUndefined,       // oldLayout
			layout,                                  // newLayout
			~( 0u ),                                 // srcQueueFamilyIndex
			queueFamily,                             // dstQueueFamilyIndex
			getTexture(),                            // image
			getSubResourceRange()                    // subresourceRange
		};
	}
}
