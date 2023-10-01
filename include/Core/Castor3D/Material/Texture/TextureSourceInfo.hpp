/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureSourceInfo_H___
#define ___C3D_TextureSourceInfo_H___

#include "TextureConfiguration.hpp"

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Data/Path.hpp>

#include <ashespp/Image/ImageCreateInfo.hpp>

namespace castor3d
{
	class TextureSourceInfo
	{
	public:
		C3D_API TextureSourceInfo() = default;
		C3D_API TextureSourceInfo( TextureSourceInfo const & rhs
			, TextureConfiguration textureConfig );
		C3D_API TextureSourceInfo( castor::String name
			, TextureConfiguration textureConfig
			, castor::Path folder
			, castor::Path relative
			, castor::ImageLoaderConfig loadConfig = { true, true, true } );
		C3D_API TextureSourceInfo( castor::String name
			, TextureConfiguration textureConfig
			, castor::String type
			, castor::ByteArray data
			, castor::ImageLoaderConfig loadConfig = { true, true, true } );
		C3D_API TextureSourceInfo( castor::String name
			, TextureConfiguration textureConfig
			, RenderTargetRPtr renderTarget );
		C3D_API TextureSourceInfo( castor::String name
			, TextureConfiguration textureConfig
			, ashes::ImageCreateInfo const & createInfo );

		bool isRenderTarget()const
		{
			return m_renderTarget != nullptr;
		}

		bool isFileImage()const
		{
			return !m_relative.empty();
		}

		bool isBufferImage()const
		{
			return !m_data.empty();
		}

		bool isVulkanImage()const
		{
			return m_createInfo->format != VK_FORMAT_UNDEFINED;
		}

		TextureConfiguration const & textureConfig()const
		{
			return m_textureConfig;
		}

		RenderTargetRPtr renderTarget()const
		{
			CU_Require( isRenderTarget() );
			return m_renderTarget;
		}

		castor::Path const & folder()const
		{
			CU_Require( isFileImage() );
			return m_folder;
		}

		castor::Path const & relative()const
		{
			CU_Require( isFileImage() );
			return m_relative;
		}

		castor::String name()const
		{
			return m_name;
		}

		castor::String const & type()const
		{
			CU_Require( isBufferImage() );
			return m_type;
		}

		castor::ByteArray const & buffer()const
		{
			CU_Require( isBufferImage() );
			return m_data;
		}

		bool allowCompression()const
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig.allowCompression;
		}

		void allowCompression( bool v )
		{
			CU_Require( isFileImage() || isBufferImage() );
			m_loadConfig.allowCompression = v;
		}

		bool allowSRGB()const
		{
			return m_loadConfig.allowSRGB;
		}

		void allowSRGB( bool v )
		{
			m_loadConfig.allowSRGB = v;
		}

		bool generateMips()const
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig.generateMips;
		}

		bool layersToTiles()const
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig.layersToTiles;
		}

		castor::ImageLoaderConfig const & loadConfig()const
		{
			CU_Require( isFileImage() || isBufferImage() );
			return m_loadConfig;
		}

		ashes::ImageCreateInfo const & createInfo()const
		{
			CU_Require( isVulkanImage() );
			return m_createInfo;
		}

		void setBuffer( castor::ByteArray data )
		{
			CU_Require( !isRenderTarget() );
			m_data = std::move( data );
		}

	private:
		castor::String m_name{};
		TextureConfiguration m_textureConfig{};
		// Render target mode.
		RenderTargetRPtr m_renderTarget{};
		// Image file mode.
		castor::Path m_folder{};
		castor::Path m_relative{};
		castor::ImageLoaderConfig m_loadConfig{};
		// Image buffer mode
		castor::String m_type{};
		castor::ByteArray m_data{};
		// Vulkan image mode.
		ashes::ImageCreateInfo m_createInfo{ {} };
	};
}

#endif
