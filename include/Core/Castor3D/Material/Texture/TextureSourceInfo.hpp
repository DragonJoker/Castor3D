/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureSourceInfo_H___
#define ___C3D_TextureSourceInfo_H___

#include "TextureModule.hpp"

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Data/Path.hpp>

#include <ashespp/Image/ImageCreateInfo.hpp>

namespace castor3d
{
	class TextureSourceInfo
	{
	public:
		C3D_API TextureSourceInfo( SamplerObs sampler
			, castor::Path folder
			, castor::Path relative
			, castor::ImageLoaderConfig loadConfig = { true, true, true } );
		C3D_API TextureSourceInfo( SamplerObs sampler
			, castor::String name
			, castor::String type
			, castor::ByteArray data
			, castor::ImageLoaderConfig loadConfig = { true, true, true } );
		C3D_API TextureSourceInfo( SamplerObs sampler
			, RenderTargetRPtr renderTarget );
		C3D_API TextureSourceInfo( SamplerObs sampler
			, ashes::ImageCreateInfo const & createInfo );

		SamplerObs sampler()const
		{
			return m_sampler;
		}

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
			CU_Require( isFileImage() || isBufferImage() );
			return ( isBufferImage()
				? m_name
				: relative().getFileName() );
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

		castor::ImageLoaderConfig const & config()const
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
		SamplerObs m_sampler{};
		// Render target mode.
		RenderTargetRPtr m_renderTarget{};
		// Image file mode.
		castor::Path m_folder{};
		castor::Path m_relative{};
		castor::ImageLoaderConfig m_loadConfig{};
		// Image buffer mode
		castor::String m_name{};
		castor::String m_type{};
		castor::ByteArray m_data{};
		// Vulkan image mode.
		ashes::ImageCreateInfo m_createInfo{ {} };
	};
}

#endif
