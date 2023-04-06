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

CU_ImplementCUSmartPtr( castor3d, TextureUnit )
CU_ImplementCUSmartPtr( castor3d, TextureUnitData )

namespace castor3d
{
	TextureUnit::TextureUnit( TextureUnit && rhs )
		: AnimableT< Engine >{ std::move( rhs ) }
		, m_data{ rhs.m_data }
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
		, m_setIndex{ std::move( rhs.m_setIndex ) }
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

	TextureUnit::TextureUnit( Engine & engine
		, TextureUnitData & data )
		: AnimableT< Engine >{ engine }
		, m_data{ data }
		, m_sampler{ engine.getDefaultSampler() }
		, m_descriptor
		{
			0u,
			0u,
			1u,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		}
		, m_changed{ false }
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()
	{
		if ( getId() != 0u )
		{
			cleanup();
		}

		auto renderTarget = m_renderTarget;

		if ( renderTarget )
		{
			getEngine()->getRenderTargetCache().remove( std::move( renderTarget ) );
		}
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
			addAnimation( std::make_unique< TextureAnimation >( *getEngine(), "Default" ) );
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
		CU_Require( hasAnimation() );
		return doGetAnimation< TextureAnimation >( "Default" );
	}

	TextureAnimation const & TextureUnit::getAnimation()const
	{
		CU_Require( hasAnimation() );
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

		bool result = false;
		auto sampler = getSampler().lock();
		CU_Require( sampler );
		sampler->initialise( device );

		if ( m_renderTarget )
		{
			std::atomic_bool isInitialised = false;
			m_renderTarget->initialise( [&isInitialised]( RenderTarget const & rt, QueueData const & queue )
				{
					isInitialised = true;
				} );

			while ( !isInitialised )
			{
				std::this_thread::sleep_for( 5_ms );
			}

			m_texture = std::make_shared< TextureLayout >( device.renderSystem
				, m_renderTarget->getTexture().image
				, m_renderTarget->getTexture().wholeViewId );
			result = true;
			m_name = cuT( "RT_" ) + castor::string::toString( m_renderTarget->getIndex() );
		}
		else if ( m_texture )
		{
			result = m_texture->initialise( device, queueData );

			if ( result
				&& m_texture->getMipmapCount() > 1u
				&& m_texture->isStatic() )
			{
				m_texture->generateMipmaps( device );
			}

			m_name = m_texture->getImage().getName();
		}

		if ( result )
		{
			m_descriptor = ashes::WriteDescriptorSet{ 0u
				, 0u
				, VkDescriptorType( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
				, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ sampler->getSampler()
					, m_texture->getDefaultView().getSampledView()
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } };
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

		if ( hasAnimation() )
		{
			getAnimation().initialiseTiles( *this );
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

			if ( m_renderTarget )
			{
				m_renderTarget->cleanup( device );
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
		if ( m_renderTarget )
		{
			return cuT( "RT_" ) + castor::string::toString( m_renderTarget->getIndex() );
		}

		CU_Require( m_texture );
		return m_name;
	}

	TextureFlagsSet TextureUnit::getFlags()const
	{
		return castor3d::getFlags( m_configuration );
	}

	bool TextureUnit::isInitialised()const
	{
		return m_texture
			&& m_texture->isInitialised();
	}

	bool TextureUnit::isTransformAnimated()const
	{
		return hasAnimation()
			&& getAnimation().isTransformAnimated();
	}

	bool TextureUnit::isTileAnimated()const
	{
		return hasAnimation()
			&& getAnimation().isTileAnimated();
	}

	void TextureUnit::setConfiguration( TextureConfiguration value )
	{
		auto format = castor::PixelFormat::eR8G8B8A8_UNORM;

		if ( m_renderTarget )
		{
			format = castor::PixelFormat( m_renderTarget->getPixelFormat() );
		}
		else if ( m_texture )
		{
			format = m_texture->getImage().getPixelFormat();
		}

		auto needsYInversion = value.needsYInversion;
		m_configuration = std::move( value );
		m_configuration.needsYInversion = m_texture
			? ( ( m_texture->needsYInversion() && needsYInversion )
				? 0u
				: ( ( m_texture->needsYInversion() || needsYInversion )
					? 1u
					: 0u ) )
			: needsYInversion;
		updateIndices( format, m_configuration );
		setTransform( m_configuration.transform );
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
		CU_Require( hasAnimation() );
		doUpdateTransform( castor::Point3f{ m_transform.translate } + translate
			, m_transform.rotate + rotate
			, castor::Point3f{ m_transform.scale } * scale );
	}

	void TextureUnit::setTexcoordSet( uint32_t value )
	{
		m_setIndex = value;
		onChanged( *this );
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
