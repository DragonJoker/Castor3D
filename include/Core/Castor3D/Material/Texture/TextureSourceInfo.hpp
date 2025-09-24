/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureSourceInfo_H___
#define ___C3D_TextureSourceInfo_H___

#include "TextureConfiguration.hpp"

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Data/Path.hpp>

#include <ashespp/Image/ImageCreateInfo.hpp>

namespace c3d
{
	struct ImageCreateParams;
}

namespace c3d
{
	class TextureSourceInfo
	{
	public:
		C3D_API TextureSourceInfo()noexcept = default;
		C3D_API TextureSourceInfo( TextureSourceInfo const & rhs
			, TextureConfiguration textureConfig );
		C3D_API TextureSourceInfo( String name
			, TextureConfiguration textureConfig
			, Path folder
			, Path relative
			, ImageLoaderConfig loadConfig = { true, true, true } );
		C3D_API TextureSourceInfo( String name
			, TextureConfiguration textureConfig
			, String type
			, ByteArray data
			, ImageLoaderConfig loadConfig = { true, true, true } );
		C3D_API TextureSourceInfo( String name
			, TextureConfiguration textureConfig
			, RenderTargetRPtr renderTarget );
		C3D_API TextureSourceInfo( String name
			, TextureConfiguration textureConfig
			, ImageCreateInfo createInfo );
		C3D_API TextureSourceInfo( String name
			, TextureConfiguration textureConfig
			, ImageCreateParams imageParams );

		bool isSerialisable()const noexcept
		{
			return m_serialisable;
		}

		bool isRenderTarget()const noexcept
		{
			return m_renderTarget != nullptr;
		}

		bool isFileImage()const noexcept
		{
			return !m_relative.empty();
		}

		bool isBufferImage()const noexcept
		{
			return !m_data.empty();
		}

		bool isVulkanImage()const noexcept
		{
			return m_createInfo.format != PixelFormat::eUNDEFINED;
		}

		TextureConfiguration const & textureConfig()const noexcept
		{
			return m_textureConfig;
		}

		RenderTargetRPtr renderTarget()const noexcept
		{
			CU_Require( isRenderTarget() );
			return m_renderTarget;
		}

		Path const & folder()const noexcept
		{
			CU_Require( isFileImage() );
			return m_folder;
		}

		Path const & relative()const noexcept
		{
			CU_Require( isFileImage() );
			return m_relative;
		}

		String name()const noexcept
		{
			return m_name;
		}

		String const & type()const noexcept
		{
			CU_Require( isBufferImage() );
			return m_type;
		}

		ByteArray const & buffer()const noexcept
		{
			CU_Require( isBufferImage() );
			return m_data;
		}

		bool allowCompression()const noexcept
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig.allowCompression;
		}

		void allowCompression( bool v )noexcept
		{
			CU_Require( isFileImage() || isBufferImage() );
			m_loadConfig.allowCompression = v;
		}

		bool keepAlphaChannel()const noexcept
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig.keepAlphaChannel;
		}

		void keepAlphaChannel( bool v )noexcept
		{
			CU_Require( isFileImage() || isBufferImage() );
			m_loadConfig.keepAlphaChannel = v;
		}

		bool allowSRGB()const noexcept
		{
			return m_loadConfig.allowSRGB;
		}

		void allowSRGB( bool v )noexcept
		{
			m_loadConfig.allowSRGB = v;
		}

		bool generateMips()const noexcept
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig.generateMips;
		}

		bool layersToTiles()const noexcept
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig.layersToTiles;
		}

		ImageLoaderConfig const & loadConfig()const noexcept
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig;
		}

		ImageCreateInfo const & createInfo()const noexcept
		{
			CU_Require( isVulkanImage() );
			return m_createInfo;
		}

		void setBuffer( ByteArray data )noexcept
		{
			CU_Require( !isRenderTarget() );
			m_data = c3d::move( data );
		}

		void setSerialisable( bool v )noexcept
		{
			m_serialisable = v;
		}

	private:
		String m_name{};
		TextureConfiguration m_textureConfig{};
		bool m_serialisable{ true };
		// Render target mode.
		RenderTargetRPtr m_renderTarget{};
		// Image file mode.
		Path m_folder{};
		Path m_relative{};
		ImageLoaderConfig m_loadConfig{};
		// Image buffer mode
		String m_type{};
		ByteArray m_data{};
		// Vulkan image mode.
		ImageCreateInfo m_createInfo{ {} };

	private:
		friend bool operator==( TextureSourceInfo const & lhs
			, TextureSourceInfo const & rhs )noexcept
		{
			if ( lhs.isVulkanImage() || rhs.isVulkanImage() )
			{
				return &lhs == &rhs;
			}

			if ( lhs.isRenderTarget() || rhs.isRenderTarget() )
			{
				return ( lhs.isRenderTarget() && rhs.isRenderTarget() )
					&& ( lhs.renderTarget() == rhs.renderTarget() );
			}

			bool result{ true };

			if ( lhs.isBufferImage() || rhs.isBufferImage() )
			{
				result = ( lhs.isBufferImage() && rhs.isBufferImage() )
					&& ( lhs.name() == rhs.name() )
					&& ( lhs.type() == rhs.type() )
					&& ( lhs.buffer().size() == rhs.buffer().size() );
			}
			else if ( lhs.isFileImage() || rhs.isFileImage() )
			{
				result = ( lhs.isFileImage() && rhs.isFileImage() )
					&& ( lhs.folder() == rhs.folder() )
					&& ( lhs.relative() == rhs.relative() );
			}

			return result
				&& ( lhs.allowCompression() == rhs.allowCompression() )
				&& ( lhs.generateMips() == rhs.generateMips() );
		}
	};
}

#endif
