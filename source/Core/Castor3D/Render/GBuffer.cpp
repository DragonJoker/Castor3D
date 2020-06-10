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
			auto & device = getCurrentRenderDevice( engine );
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

	TextureUnit GBufferBase::doInitialiseTexture( Engine & engine
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, VkFormat format
		, VkImageUsageFlags usageFlags
		, VkBorderColor const & borderColor )
	{
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
			createFlags | ( size.depth > 1u
				? VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
				: 0u ),
			( size.depth > 1u
				? VK_IMAGE_TYPE_3D
				: VK_IMAGE_TYPE_2D ),
			getFormat( engine, format ),
			size,
			1u,
			( size.depth > 1u
				? 1u
				: layerCount ),
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			usageFlags,
		};
		auto layout = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name );
		TextureUnit unit{ engine };
		unit.setTexture( layout );
		unit.setSampler( sampler );
		layout->forEachView( []( TextureViewUPtr const & view )
			{
				view->initialiseSource();
			} );
		return unit;
	}

	//*********************************************************************************************
}
