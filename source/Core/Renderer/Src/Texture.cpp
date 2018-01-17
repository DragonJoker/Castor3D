/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Texture.hpp"

#include "StagingBuffer.hpp"

namespace renderer
{
	Texture::Texture( Device const & device )
		: m_device{ device }
	{
	}

	void Texture::setImage( PixelFormat format
		, IVec2 const & size
		, ByteArray const & data
		, StagingBuffer const & stagingBuffer
		, CommandBuffer const & commandBuffer
		, ImageUsageFlags usageFlags
		, ImageTiling tiling )
	{
		setImage( format
			, size
			, usageFlags
			, tiling );
		stagingBuffer.copyTextureData( commandBuffer
			, data
			, *this );
	}
}
