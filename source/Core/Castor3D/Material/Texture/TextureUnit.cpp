#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementSmartPtr( castor3d, TextureData )
CU_ImplementSmartPtr( castor3d, TextureUnit )
CU_ImplementSmartPtr( castor3d, TextureUnitData )

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
		, m_descriptor{ std::move( rhs.m_descriptor ) }
		, m_id{ std::move( rhs.m_id ) }
		, m_name{ std::move( rhs.m_name ) }
		, m_initialised{ std::move( rhs.m_initialised ) }
		, m_animated{ std::move( rhs.m_animated ) }
		, m_setIndex{ std::move( rhs.m_setIndex ) }
	{
		getOwner()->getMaterialCache().unregisterUnit( rhs );
		rhs.m_id = 0u;
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
		, m_descriptor{ 0u, 0u, 1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()
	{
		if ( getId() != 0u )
		{
			cleanup();
		}

		if ( m_data.base->sourceInfo.isRenderTarget() )
		{
			auto renderTarget = m_data.base->sourceInfo.renderTarget();
			getEngine()->getRenderTargetCache().remove( renderTarget );
		}
	}

	void TextureUnit::setTexture( Texture const * texture )
	{
		m_texture = texture;
	}

	TextureAnimation & TextureUnit::createAnimation()
	{
		if ( !hasAnimation() )
		{
			addAnimation( castor::makeUniqueDerived< Animation, TextureAnimation >( *getEngine(), "Default" ) );
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

	bool TextureUnit::initialise()
	{
		if ( m_initialised && isInitialised() )
		{
			getOwner()->getMaterialCache().registerUnit( *this );
			return m_initialised;
		}

		bool result = false;
		m_device = getEngine()->getRenderDevice();

		if ( m_data.base->sourceInfo.isRenderTarget() )
		{
			m_name = m_texture->imageId.data->name;
			result = true;
		}
		else if ( m_texture )
		{
			m_name = getCPUImage().getName();
			result = true;
		}

		if ( result )
		{
			m_gpuImage = m_texture->image.get();
			m_descriptor = ashes::WriteDescriptorSet{ 0u
				, 0u
				, VkDescriptorType( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
				, ashes::VkDescriptorImageInfoArray{ VkDescriptorImageInfo{ getSampler()
					, m_texture->sampledView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } };
		}

		if ( hasAnimation() )
		{
			getAnimation().initialiseTiles( *this );
		}

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

			if ( m_data.base->sourceInfo.isRenderTarget() )
			{
				auto renderTarget = m_data.base->sourceInfo.renderTarget();
				renderTarget->cleanup( device );
			}
		}
	}

	VkImageType TextureUnit::getType()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.imageType;
	}

	castor::String TextureUnit::toString()const
	{
		return m_name;
	}

	TextureFlagsSet TextureUnit::getFlags()const
	{
		return castor3d::getFlags( m_configuration );
	}

	bool TextureUnit::isInitialised()const
	{
		return m_texture && *m_texture;
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

	ashes::Sampler const & TextureUnit::getSampler()const
	{
		CU_Require( isTextured() );
		return *m_texture->sampler;
	}

	RenderTargetRPtr TextureUnit::getRenderTarget()const
	{
		CU_Require( isRenderTarget() );
		return m_data.base->sourceInfo.renderTarget();
	}

	castor::String TextureUnit::getTextureName()const
	{
		CU_Require( isTextured() );
		return m_name;
	}

	castor::Path TextureUnit::getTexturePath()const
	{
		CU_Require( isTextured() );
		return getCPUImage().getPath();
	}

	bool TextureUnit::isTextureStatic()const
	{
		return m_data.base->sourceInfo.isBufferImage()
			|| m_data.base->sourceInfo.isFileImage();
	}

	VkFormat TextureUnit::getTexturePixelFormat()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.format;
	}

	castor::Point3ui TextureUnit::getTextureImageTiles()const
	{
		return getCPUImage().getPxBuffer().getTiles();
	}

	bool TextureUnit::hasTextureImageBuffer()const
	{
		return getCPUImage().hasBuffer();
	}

	castor::PxBufferBase const & TextureUnit::getTextureImageBuffer()const
	{
		return getCPUImage().getPxBuffer();
	}

	VkExtent3D TextureUnit::getTextureDimensions()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.extent;
	}

	uint32_t TextureUnit::getTextureMipmapCount()const
	{
		CU_Require( isTextured() );
		return m_texture->imageId.data->info.mipLevels;
	}

	void TextureUnit::setConfiguration( TextureConfiguration value )
	{
		auto format = castor::PixelFormat::eR8G8B8A8_UNORM;

		if ( m_data.base->sourceInfo.isRenderTarget() )
		{
			auto renderTarget = m_data.base->sourceInfo.renderTarget();
			format = castor::PixelFormat( renderTarget->getPixelFormat() );
		}
		else if ( m_texture )
		{
			format = convert( getTexturePixelFormat() );
		}

		auto needsYInversion = value.needsYInversion;
		auto flippedPixels = m_data.base->image
			? m_data.base->image->getPixels()->isFlipped()
			: needsYInversion;
		m_configuration = std::move( value );
		m_configuration.needsYInversion = ( ( flippedPixels && needsYInversion )
			? 0u
			: ( ( flippedPixels || needsYInversion ) ? 1u : 0u ) );
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
