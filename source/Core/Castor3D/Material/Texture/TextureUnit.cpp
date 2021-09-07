#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementCUSmartPtr( castor3d, TextureUnit );

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
			SamplerResPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name, engine );
				sampler.lock()->setMinFilter( VK_FILTER_LINEAR );
				sampler.lock()->setMagFilter( VK_FILTER_LINEAR );
				sampler.lock()->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler.lock()->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler.lock()->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler.lock()->setBorderColour( VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK );
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
			unit.initialise( device, *device.graphicsData() );
			return unit;
		}
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( TextureUnit && rhs )
		: AnimableT< Engine >{ std::move( rhs ) }
		, m_device{ std::move( rhs.m_device ) }
		, m_configuration{ std::move( rhs.m_configuration ) }
		, m_transform{ std::move( rhs.m_transform ) }
		, m_transformations{ std::move( rhs.m_transformations ) }
		, m_texture{ std::move( rhs.m_texture ) }
		, m_renderTarget{ std::move( rhs.m_renderTarget ) }
		, m_sampler{ std::move( rhs.m_sampler ) }
		, m_ownSampler{ std::move( rhs.m_ownSampler ) }
		, m_descriptor{ std::move( rhs.m_descriptor ) }
		, m_id{ std::move( rhs.m_id ) }
		, m_changed{ std::move( rhs.m_changed ) }
		, m_name{ std::move( rhs.m_name ) }
		, m_initialised{ std::move( rhs.m_initialised ) }
		, m_animated{ std::move( rhs.m_animated ) }
	{
		getOwner()->getMaterialCache().unregisterUnit( rhs );
		rhs.m_id = 0u;
		rhs.m_changed = false;
		rhs.m_initialised = false;
		rhs.m_animated = false;

		if ( m_id )
		{
			m_id = 0u;
			getOwner()->getMaterialCache().registerUnit( *this );
		}
	}

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

		CU_Assert( getId() == 0u, "Did you forget to call TextureUnit::cleanup ?" );
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

	bool TextureUnit::initialise( RenderDevice const & device
		, QueueData const & queueData )
	{
		if ( m_initialised && isInitialised() )
		{
			getOwner()->getMaterialCache().registerUnit( *this );
			return m_initialised;
		}

		RenderTargetSPtr target = m_renderTarget.lock();
		bool result = false;

		if ( target )
		{
			target->initialise( device, queueData );
			m_texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
				, target->getTexture().image
				, target->getTexture().wholeViewId );
			result = true;
			m_name = cuT( "RT_" ) + castor::string::toString( target->getIndex() );
		}
		else if ( m_texture )
		{
			result = m_texture->initialise( device, queueData );
			auto sampler = getSampler().lock();
			CU_Require( sampler );
			sampler->initialise( device );

			if ( result
				&& m_texture->getMipmapCount() > 1u
				&& m_texture->isStatic() )
			{
				m_texture->generateMipmaps( device );
			}

			m_descriptor = { 0u
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, { { sampler->getSampler()
					, m_texture->getDefaultView().getSampledView()
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } };
			m_name = m_texture->getImage().getName();
		}

		if ( m_texture )
		{
			log::info << "Loaded texture [" << toString() << "] image (" << *m_texture << ")" << std::endl;
		}
		else
		{
			CU_Failure( "Couldn't load texture" );
			log::error << "Couldn't load texture [" << toString() << "]" << std::endl;
		}

		m_device = &device;
		m_initialised = result;
		getOwner()->getMaterialCache().registerUnit( *this );
		return result;
	}

	void TextureUnit::cleanup()
	{
		getOwner()->getMaterialCache().unregisterUnit( *this );
		m_id = 0u;

		if ( m_device )
		{
			auto & device = *m_device;
			m_device = nullptr;
			auto sampler = getSampler().lock();

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
		doUpdateStartIndex( m_configuration.metalnessMask, format );
		doUpdateStartIndex( m_configuration.glossinessMask, format );
		doUpdateStartIndex( m_configuration.roughnessMask, format );
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
