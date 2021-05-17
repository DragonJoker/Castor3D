#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

namespace castor3d
{
	namespace
	{
		void doUpdateStartIndex( castor::Point2ui & mask
			, castor::PixelFormat format )
		{
			if ( mask[0] )
			{
				auto components = getPixelComponents( mask[0] );

				switch ( components.size() )
				{
				case 1:
					mask[1] = castor::getComponentIndex( *components.begin(), format );
					break;
				case 3:
					if ( mask[0] & 0xFF000000 )
					{
						mask[1] = 1;
					}
					else
					{
						mask[1] = 0;
					}
					break;
				default:
					CU_Failure( "Invalid component count for a texture component flag" );
					mask[1] = 0;
					break;
				}
			}
		}

		TextureUnit createTextureUnit( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, VkExtent3D size
			, uint32_t arrayLayers
			, VkImageType imageType
			, VkImageCreateFlags createFlags
			, VkImageUsageFlags usageFlags )
		{
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
				sampler->setBorderColour( VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK );
			}

			ashes::ImageCreateInfo image
			{
				createFlags,
				imageType,
				format,
				size,
				1u,
				arrayLayers,
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
			unit.initialise( device );
			return unit;
		}
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( Engine & engine )
		: AnimableT< Engine >{ engine }
		, m_changed{ false }
		, m_sampler{ engine.getDefaultSampler() }
		, m_descriptor
		{
			0u,
			0u,
			1u,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		}
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()
	{
		auto renderTarget = m_renderTarget.lock();

		if ( renderTarget )
		{
			getEngine()->getRenderTargetCache().remove( std::move( renderTarget ) );
		}
	}

	TextureUnit TextureUnit::create( Engine & engine
		, RenderDevice const & device
		, castor::String const & name
		, VkFormat format
		, uint32_t size
		, VkImageCreateFlags createFlags
		, VkImageUsageFlags usageFlags )
	{
		return createTextureUnit( engine
			, device
			, name
			, format
			, { size, 1u, 1u }
			, 1u
			, VK_IMAGE_TYPE_1D
			, createFlags
			, usageFlags );
	}

	TextureUnit TextureUnit::create( Engine & engine
		, RenderDevice const & device
		, castor::String const & name
		, VkFormat format
		, uint32_t size
		, uint32_t arrayLayers
		, VkImageCreateFlags createFlags
		, VkImageUsageFlags usageFlags )
	{
		return createTextureUnit( engine
			, device
			, name
			, format
			, { size, 1u, 1u }
			, arrayLayers
			, VK_IMAGE_TYPE_1D
			, createFlags
			, usageFlags );
	}

	TextureUnit TextureUnit::create( Engine & engine
		, RenderDevice const & device
		, castor::String const & name
		, VkFormat format
		, VkExtent2D const & size
		, VkImageCreateFlags createFlags
		, VkImageUsageFlags usageFlags )
	{
		return createTextureUnit( engine
			, device
			, name
			, format
			, { size.width, size.height, 1u }
			, 1u
			, VK_IMAGE_TYPE_2D
			, createFlags
			, usageFlags );
	}

	TextureUnit TextureUnit::create( Engine & engine
		, RenderDevice const & device
		, castor::String const & name
		, VkFormat format
		, VkExtent2D const & size
		, uint32_t arrayLayers
		, VkImageCreateFlags createFlags
		, VkImageUsageFlags usageFlags )
	{
		return createTextureUnit( engine
			, device
			, name
			, format
			, { size.width, size.height, 1u }
			, arrayLayers
			, VK_IMAGE_TYPE_2D
			, createFlags
			, usageFlags );
	}

	TextureUnit TextureUnit::create( Engine & engine
		, RenderDevice const & device
		, castor::String const & name
		, VkFormat format
		, VkExtent3D const & size
		, VkImageCreateFlags createFlags
		, VkImageUsageFlags usageFlags )
	{
		return createTextureUnit( engine
			, device
			, name
			, format
			, size
			, 1u
			, VK_IMAGE_TYPE_3D
			, createFlags
			, usageFlags );
	}

	void TextureUnit::setTexture( TextureLayoutSPtr texture )
	{
		m_texture = texture;
		m_changed = true;

		if ( m_texture )
		{
			m_name = m_texture->getImage().getName();
		}
	}

	TextureAnimation & TextureUnit::createAnimation()
	{
		if ( !hasAnimation() )
		{
			addAnimation( std::make_unique< TextureAnimation >( *this, "Default" ) );
			m_animated = true;
		}

		return doGetAnimation< TextureAnimation >( "Default" );
	}

	void TextureUnit::removeAnimation()
	{
		if ( hasAnimation() )
		{
			doRemoveAnimation( "Default" );
		}
	}

	TextureAnimation & TextureUnit::getAnimation()
	{
		CU_Require( m_animated );
		return doGetAnimation< TextureAnimation >( "Default" );
	}

	TextureAnimation const & TextureUnit::getAnimation()const
	{
		CU_Require( m_animated );
		return doGetAnimation< TextureAnimation >( "Default" );
	}

	bool TextureUnit::initialise( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			return m_initialised;
		}

		RenderTargetSPtr target = m_renderTarget.lock();
		bool result = false;

		if ( target )
		{
			target->initialise( device );
			m_texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
				, target->getTexture() );
			result = true;
			m_name = cuT( "RT_" ) + castor::string::toString( target->getIndex() );
		}
		else if ( m_texture )
		{
			result = m_texture->initialise( device );
			auto sampler = getSampler();
			CU_Require( sampler );
			sampler->initialise( device );

			if ( result
				&& m_texture->getMipmapCount() > 1u
				&& m_texture->isStatic() )
			{
				m_texture->generateMipmaps( device );
			}

			m_descriptor = ashes::WriteDescriptorSet
			{
				0u,
				0u,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				{
					{
						sampler->getSampler(),
						m_texture->getDefaultView().getSampledView(),
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					}
				}
			};
			m_name = m_texture->getImage().getName();
		}

		if ( m_texture )
		{
			log::info << "Loaded texture [" << toString() << "] image (" << *m_texture << ")" << std::endl;
		}

		m_device = &device;
		m_initialised = result;
		return result;
	}

	void TextureUnit::cleanup()
	{
		if ( m_device )
		{
			auto & device = *m_device;
			m_device = nullptr;
			auto sampler = getSampler();

			if ( sampler )
			{
				sampler->cleanup();
			}

			if ( m_texture )
			{
				m_texture->cleanup();
			}

			RenderTargetSPtr target = m_renderTarget.lock();

			if ( target )
			{
				target->cleanup( device );
			}
		}
	}

	VkImageType TextureUnit::getType()const
	{
		CU_Require( m_texture );
		return m_texture->getType();
	}

	castor::String TextureUnit::toString()const
	{
		auto renderTarget = m_renderTarget.lock();

		if ( renderTarget )
		{
			return cuT( "RT_" ) + castor::string::toString( renderTarget->getIndex() );
		}

		CU_Require( m_texture );
		return m_name;
	}

	TextureFlags TextureUnit::getFlags()const
	{
		return castor3d::getFlags( m_configuration );
	}

	bool TextureUnit::isInitialised()const
	{
		return m_texture
			&& m_texture->isInitialised();
	}

	void TextureUnit::setConfiguration( TextureConfiguration value )
	{
		RenderTargetSPtr target = m_renderTarget.lock();
		auto format = castor::PixelFormat::eR8G8B8A8_UNORM;

		if ( target )
		{
			format = castor::PixelFormat( target->getPixelFormat() );
		}
		else if ( m_texture )
		{
			format = m_texture->getImage().getPixelFormat();
		}

		m_configuration = std::move( value );
		m_configuration.needsYInversion = m_texture
			? ( m_texture->needsYInversion()
				? 1u
				: 0u )
			: 0u;
		doUpdateStartIndex( m_configuration.colourMask, format );
		doUpdateStartIndex( m_configuration.specularMask, format );
		doUpdateStartIndex( m_configuration.glossinessMask, format );
		doUpdateStartIndex( m_configuration.opacityMask, format );
		doUpdateStartIndex( m_configuration.emissiveMask, format );
		doUpdateStartIndex( m_configuration.normalMask, format );
		doUpdateStartIndex( m_configuration.heightMask, format );
		doUpdateStartIndex( m_configuration.occlusionMask, format );
		doUpdateStartIndex( m_configuration.transmittanceMask, format );
		onChanged( *this );
	}

	void TextureUnit::setTransform( castor::Point3f const & translate
		, castor::Angle const & rotate
		, castor::Point3f const & scale )
	{
		m_transform.translate->x = translate->x;
		m_transform.translate->y = translate->y;
		m_transform.translate->z = translate->z;

		m_transform.rotate = rotate;

		m_transform.scale->x = scale->x;
		m_transform.scale->y = scale->y;
		m_transform.scale->z = scale->z;

		doUpdateTransform( translate
			, rotate
			, scale );
	}

	void TextureUnit::setTransform( TextureTransform const & transform )
	{
		m_transform = transform;
		doUpdateTransform( castor::Point3f{ m_transform.translate }
			, m_transform.rotate
			, castor::Point3f{ m_transform.scale } );
	}

	void TextureUnit::setAnimationTransform( castor::Point3f const & translate
		, castor::Angle const & rotate
		, castor::Point3f const & scale )
	{
		doUpdateTransform( castor::Point3f{ m_transform.translate } + translate
			, m_transform.rotate + rotate
			, castor::Point3f{ m_transform.scale } * scale );
	}

	void TextureUnit::doUpdateTransform( castor::Point3f const & translate
		, castor::Angle const & rotate
		, castor::Point3f const & scale )
	{
		m_configuration.transform.translate->x = translate->x;
		m_configuration.transform.translate->y = translate->y;
		m_configuration.transform.translate->z = translate->z;

		m_configuration.transform.rotate = rotate;

		m_configuration.transform.scale->x = scale->x;
		m_configuration.transform.scale->y = scale->y;
		m_configuration.transform.scale->z = scale->z;

		onChanged( *this );
	}
}
