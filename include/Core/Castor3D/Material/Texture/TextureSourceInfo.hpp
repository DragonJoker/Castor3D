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
		C3D_API TextureSourceInfo( SamplerRes sampler
			, castor::Path folder
			, castor::Path relative
			, castor::ImageLoaderConfig loadConfig = { true, true, true } );
		C3D_API TextureSourceInfo( SamplerRes sampler
			, RenderTargetSPtr renderTarget );
		C3D_API TextureSourceInfo( SamplerRes sampler
			, ashes::ImageCreateInfo const & createInfo );

		SamplerRes sampler()const
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

		bool isVulkanImage()const
		{
			return m_createInfo->format != VK_FORMAT_UNDEFINED;
		}

		RenderTargetSPtr renderTarget()const
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

		bool allowCompression()const
		{
			CU_Require( isFileImage() );
			return m_loadConfig.allowCompression;
		}

		bool generateMips()const
		{
			CU_Require( isFileImage() );
			return m_loadConfig.generateMips;
		}

		bool layersToTiles()const
		{
			CU_Require( isFileImage() );
			return m_loadConfig.layersToTiles;
		}

		castor::ImageLoaderConfig const & config()const
		{
			CU_Require( isFileImage() );
			return m_loadConfig;
		}

		ashes::ImageCreateInfo const & createInfo()const
		{
			CU_Require( !isRenderTarget() );
			CU_Require( !isFileImage() );
			return m_createInfo;
		}

	private:
		SamplerRes m_sampler{};
		// Render target mode.
		RenderTargetSPtr m_renderTarget{};
		// Image file mode.
		castor::Path m_folder{};
		castor::Path m_relative{};
		castor::ImageLoaderConfig m_loadConfig{};
		// Vulkan image mode.
		ashes::ImageCreateInfo m_createInfo{ {} };
	};
}

#endif
