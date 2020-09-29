#include "Castor3D/Render/GBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		VkFormat getDepthFormat( Engine & engine
			, VkFormat format )
		{
			auto & device = *engine.getRenderSystem()->getMainRenderDevice();
			std::vector< VkFormat > depthFormats
			{
				format,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM_S8_UINT,
			};
			return device.selectSuitableFormat( depthFormats
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}
	}

	//*********************************************************************************************

	TextureUnit GBufferBase::doCreateTexture( Engine & engine
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, uint32_t mipLevels
		, VkFormat format
		, VkImageUsageFlags usageFlags
		, VkBorderColor const & borderColor )
	{
		mipLevels = std::max( 1u, mipLevels );
		auto getFormat = []( Engine & engine
			, VkFormat format )
		{
			return ashes::isDepthOrStencilFormat( format )
				? getDepthFormat( engine, format )
				: format;
		};

		SamplerSPtr sampler;

		if ( engine.getSamplerCache().has( name ) )
		{
			sampler = engine.getSamplerCache().find( name );
		}
		else
		{
			sampler = engine.getSamplerCache().add( name );
			sampler->setMinFilter( VK_FILTER_LINEAR );
			sampler->setMagFilter( VK_FILTER_LINEAR );
			sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setBorderColour( borderColor );
		}

		ashes::ImageCreateInfo image
		{
			createFlags
				| ( size.depth > 1u
					? VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
					: 0u ),
			( size.depth > 1u
				? VK_IMAGE_TYPE_3D
				: VK_IMAGE_TYPE_2D ),
			getFormat( engine, format ),
			size,
			mipLevels,
			( size.depth > 1u
				? 1u
				: layerCount ),
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			usageFlags
				| ( mipLevels > 1u
					? ( VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT )
					: 0u ),
		};
		auto layout = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name );
		TextureUnit unit{ engine };
		unit.setTexture( layout );
		unit.setSampler( sampler );
		return unit;
	}

	//*********************************************************************************************
}
