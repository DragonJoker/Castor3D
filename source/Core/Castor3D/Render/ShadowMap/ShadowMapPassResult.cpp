#include "Castor3D/Render/ShadowMap/ShadowMapPassResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		VkFormat doGetDepthFormat( Engine & engine )
		{
			static VkFormat const result
			{
				[&engine]()
				{
					auto & device = getCurrentRenderDevice( engine );
					std::vector< VkFormat > depthFormats
					{
						VK_FORMAT_D24_UNORM_S8_UINT,
						VK_FORMAT_D16_UNORM_S8_UINT,
					};
					return device.selectSuitableFormat( depthFormats
						, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
				}()
			};
			return result;
		}

		TextureUnit doInitialiseDepth( Engine & engine
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, Size const & size
			, uint32_t layerCount )
		{
			String const name = prefix + cuT( "ShadowMapDepth" );
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
			}

			ashes::ImageCreateInfo depth
			{
				createFlags,
				VK_IMAGE_TYPE_2D,
				doGetDepthFormat( engine ),
				{ size[0], size[1], 1u },
				1u,
				layerCount,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			};
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, std::move( depth )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			texture->forEachView( []( TextureViewUPtr const & view )
				{
					view->initialiseSource();
				} );
			return unit;
		}

		TextureUnit doInitialiseTexture( Engine & engine
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, Size const & size
			, uint32_t layerCount
			, SmTexture texture )
		{
			String const name = prefix + cuT( "ShadowMap" ) + getName( texture );
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
				sampler->setBorderColour( VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE );
			}

			ashes::ImageCreateInfo image
			{
				createFlags,
				VK_IMAGE_TYPE_2D,
				getTextureFormat( texture ),
				{ size[0], size[1], 1u },
				1u,
				layerCount,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
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

		TextureUnitArray createTextures( Engine & engine
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, castor::Size const & size 
			, uint32_t layerCount )
		{
			TextureUnitArray result;
			result.emplace_back( doInitialiseDepth( engine, prefix, createFlags, size, layerCount ) );
			result.emplace_back( doInitialiseTexture( engine, prefix, createFlags, size, layerCount, SmTexture::eLinearNormal ) );
			result.emplace_back( doInitialiseTexture( engine, prefix, createFlags, size, layerCount, SmTexture::eVariance ) );
			result.emplace_back( doInitialiseTexture( engine, prefix, createFlags, size, layerCount, SmTexture::ePosition ) );
			result.emplace_back( doInitialiseTexture( engine, prefix, createFlags, size, layerCount, SmTexture::eFlux ) );
			return result;
		}
	}

	//*********************************************************************************************

	castor::String getName( SmTexture texture )
	{
		static std::array< String, size_t( SmTexture::eCount ) > Values
		{
			{
				cuT( "Depth" ),
				cuT( "Linear" ),
				cuT( "Variance" ),
				cuT( "Position" ),
				cuT( "NormalFlux" ),
			}
		};

		return Values[size_t( texture )];
	}

	castor::String getTextureName( LightType light
		, SmTexture texture )
	{
		return cuT( "c3d_" ) + getName( light ) + getName( texture );
	}

	VkFormat getTextureFormat( SmTexture texture )
	{
		static std::array< VkFormat, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_R32_SFLOAT,
				VK_FORMAT_R32G32_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};
		CU_Require( texture != SmTexture::eDepth
			&& "You can't use this function for depth texture format" );
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	ShadowMapPassResult::ShadowMapPassResult( Engine & engine
		, castor::String const & prefix
		, VkImageCreateFlags createFlags
		, castor::Size const & size
		, uint32_t layerCount )
		: m_owned{ createTextures( engine, prefix, createFlags, size, layerCount ) }
	{
	}

	//*********************************************************************************************
}
