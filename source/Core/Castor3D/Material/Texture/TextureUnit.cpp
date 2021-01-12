#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		String writeMask( uint32_t mask )
		{
			auto stream = castor::makeStringStream();
			stream << cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << mask;
			return stream.str();
		}

		void doUpdateShift( castor::Point2ui & mask )
		{
			if ( mask[0] )
			{
				for ( uint32_t i = 0u; i < 4u; ++i )
				{
					if ( mask[0] & ( 0x000000FFu << ( i * 8u ) ) )
					{
						mask[1] = i;
						i = 4u;
					}
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

	TextureUnit::TextWriter::TextWriter( String const & tabs, MaterialType type )
		: castor::TextWriter< TextureUnit >{ tabs, "TextureUnit" }
		, m_type{ type }
	{
	}

	bool TextureUnit::TextWriter::operator()( TextureUnit const & unit, TextFile & file )
	{
		bool result = true;
		auto hasTexture = unit.isTextured() && unit.getTexture();

		if ( hasTexture )
		{
			auto texture = unit.getTexture();
			auto image = texture->getPath();
			hasTexture = !image.empty() || !texture->isStatic();

			if ( hasTexture )
			{
				if ( auto block = writeHeader( cuT( "texture_unit" ), file ) )
				{
					if ( unit.getSampler() && unit.getSampler()->getName() != cuT( "Default" ) )
					{
						result = writeName( cuT( "sampler" ), unit.getSampler()->getName(), file );
					}

					if ( result && unit.getTexture()->getMipmapCount() > 1 )
					{
						result = write( cuT( "levels_count" ), unit.getTexture()->getMipmapCount(), file );
					}

					if ( result )
					{
						if ( !texture->isStatic() )
						{
							if ( unit.getRenderTarget() )
							{
								result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *unit.getRenderTarget(), file );
								checkError( result, "render_target" );
							}
							else
							{
								// Procedurally generated textures, certainly will go here
							}
						}
						else
						{
							result = writeFile( cuT( "image" ), Path{ image }, cuT( "Textures" ), file );
						}
					}

					if ( result )
					{
						result = TextureConfiguration::TextWriter{ m_tabs, m_type }( unit.getConfiguration(), file );
						checkError( result, "configuration" );
					}
				}
			}
		}

		return result;
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
			doAddAnimation( std::make_unique< TextureAnimation >( *this, "Default" ) );
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
			m_texture = target->getTexture().getTexture();
			result = true;
			m_name = cuT( "RT_" ) + castor::string::toString( target->getIndex() );
		}
		else if ( m_texture )
		{
			result = m_texture->initialise( device );
			auto sampler = getSampler();
			CU_Require( sampler );
			sampler->initialise( device );

			if ( result && m_texture->getMipmapCount() > 1u )
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
		m_configuration = std::move( value );
		m_configuration.needsYInversion = m_texture
			? ( m_texture->needsYInversion()
				? 1u
				: 0u )
			: 0u;
		doUpdateShift( m_configuration.colourMask );
		doUpdateShift( m_configuration.specularMask );
		doUpdateShift( m_configuration.glossinessMask );
		doUpdateShift( m_configuration.opacityMask );
		doUpdateShift( m_configuration.emissiveMask );
		doUpdateShift( m_configuration.normalMask );
		doUpdateShift( m_configuration.heightMask );
		doUpdateShift( m_configuration.occlusionMask );
		doUpdateShift( m_configuration.transmittanceMask );
		onChanged( *this );
	}

	void TextureUnit::setTransform( castor::Point3f const & translate
		, castor::Angle const & rotate
		, castor::Point3f const & scale )
	{
		m_configuration.translate->x = translate->x;
		m_configuration.translate->y = translate->y;
		m_configuration.translate->z = translate->z;
		m_configuration.translate->w = rotate.cos();

		m_configuration.scale->x = 1.0f + scale->x;
		m_configuration.scale->y = 1.0f + scale->y;
		m_configuration.scale->z = 1.0f + scale->z;
		m_configuration.scale->w = rotate.sin();

		onChanged( *this );
	}
}
