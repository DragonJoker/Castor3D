#include "Image/GlTextureView.hpp"

#include "Core/GlDevice.hpp"
#include "Image/GlTexture.hpp"

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
}
