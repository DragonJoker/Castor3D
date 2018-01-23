/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Image/Texture.hpp"

#include "Buffer/StagingBuffer.hpp"
#include "Image/TextureView.hpp"

namespace renderer
{
	Texture::Texture( Texture && rhs )
		: m_device{ rhs.m_device }
		, m_format{ rhs.m_format }
		, m_type{ rhs.m_type }
		, m_size{ rhs.m_size }
		, m_layerCount{ rhs.m_layerCount }
		, m_samples{ rhs.m_samples }
	{
	}

	Texture & Texture::operator=( Texture && rhs )
	{
		if ( &rhs != this )
		{
			m_format = rhs.m_format;
			m_type = rhs.m_type;
			m_size = rhs.m_size;
			m_layerCount = rhs.m_layerCount;
			m_samples = rhs.m_samples;
		}

		return *this;
	}

	Texture::Texture( Device const & device )
		: m_device{ device }
	{
	}

	void Texture::setImage( PixelFormat format
		, uint32_t size
		, ImageUsageFlags usageFlags
		, ImageTiling tiling )
	{
		m_format = format;
		m_type = TextureType::e1D;
		m_size = UIVec3{ size, 0u, 0u };
		m_layerCount = 0u;
		m_samples = SampleCountFlag::e1;
		doSetImage1D( usageFlags
			, tiling
			, renderer::MemoryPropertyFlag::eDeviceLocal );
	}

	void Texture::setImage( PixelFormat format
		, UIVec2 const & size
		, ImageUsageFlags usageFlags
		, ImageTiling tiling
		, SampleCountFlag samples )
	{
		m_format = format;
		m_type = TextureType::e2D;
		m_size = UIVec3{ size[0], size[1], 0u };
		m_layerCount = 0;
		m_samples = samples;
		doSetImage2D( usageFlags
			, tiling
			, renderer::MemoryPropertyFlag::eDeviceLocal );
	}

	void Texture::setImage( PixelFormat format
		, UIVec3 const & size
		, ImageUsageFlags usageFlags
		, ImageTiling tiling
		, SampleCountFlag samples )
	{
		m_format = format;
		m_type = TextureType::e3D;
		m_size = size;
		m_layerCount = 0;
		m_samples = samples;
		doSetImage3D( usageFlags
			, tiling
			, renderer::MemoryPropertyFlag::eDeviceLocal );
	}

	void Texture::setImage( PixelFormat format
		, uint32_t size
		, uint32_t layerCount
		, ImageUsageFlags usageFlags
		, ImageTiling tiling )
	{
		m_format = format;
		m_type = TextureType::e1DArray;
		m_size = UIVec3{ size, 0u, 0u };
		m_layerCount = layerCount;
		m_samples = SampleCountFlag::e1;
		doSetImage1D( usageFlags
			, tiling
			, renderer::MemoryPropertyFlag::eDeviceLocal );
	}

	void Texture::setImage( PixelFormat format
		, UIVec2 const & size
		, uint32_t layerCount
		, ImageUsageFlags usageFlags
		, ImageTiling tiling )
	{
		m_format = format;
		m_type = TextureType::e2DArray;
		m_size = UIVec3{ size[0], size[1], 0u };
		m_layerCount = layerCount;
		m_samples = SampleCountFlag::e1;
		doSetImage2D( usageFlags
			, tiling
			, renderer::MemoryPropertyFlag::eDeviceLocal );
	}
}
