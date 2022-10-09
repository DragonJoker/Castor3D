#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace texcfgbuf
	{
		static TextureConfigurationBuffer::TextureConfigurationsData doBindData( uint8_t * buffer
			, VkDeviceSize size
			, uint32_t count )
		{
			CU_Require( ( count * sizeof( TextureConfigurationBuffer::Data ) ) <= size );
			return castor::makeArrayView( reinterpret_cast< TextureConfigurationBuffer::Data * >( buffer )
				, reinterpret_cast< TextureConfigurationBuffer::Data * >( buffer ) + count );
		}

		static castor::Point2f writeFlags( castor::Point2ui const & a )
		{
			return castor::Point2f
			{
				a[0] ? 1.0f : 0.0f,
				float( a[1] ),
			};
		}

		static castor::Point2f writeFlags( float a
			, float b = 0.0f )
		{
			return castor::Point2f
			{
				a,
				b,
			};
		}

		static castor::Point2ui writeFlags( uint32_t a
			, uint32_t b = 0u )
		{
			return castor::Point2ui
			{
				a,
				b,
			};
		}

		static uint32_t writeBool( bool v )
		{
			return v ? 1u : 0u;
		}
	}

	//*********************************************************************************************

	TextureConfigurationBuffer::TextureConfigurationBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "TextureConfigurationBuffer" ) }
		, m_data{ texcfgbuf::doBindData( m_buffer.getPtr(), m_buffer.getSize(), count ) }
	{
	}

	uint32_t TextureConfigurationBuffer::addTextureConfiguration( TextureUnit & unit )
	{
		if ( unit.getId() == 0u )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			auto & device = getDevice();

			if ( device.hasBindless() )
			{
				m_configurations.push_back( unit.getConfiguration() );
				unit.setId( uint32_t( m_configurations.size() ) );
			}
			else
			{
				auto & config = unit.getConfiguration();
				auto it = unit.hasAnimation()
					? m_configurations.end()
					: std::find( m_configurations.begin()
						, m_configurations.end()
						, config );

				if ( it == m_configurations.end() )
				{
					m_configurations.push_back( config );
					it = std::next( m_configurations.begin()
						, ptrdiff_t( m_configurations.size() - 1u ) );
				}

				unit.setId( uint32_t( std::distance( m_configurations.begin(), it ) ) + 1u );
			}

			m_connections.emplace_back( unit.onChanged.connect( [this]( TextureUnit const & punit )
				{
					m_dirty.emplace_back( &punit );
				} ) );
			m_dirty.emplace_back( &unit );
		}
		return unit.getId();
	}

	void TextureConfigurationBuffer::removeTextureConfiguration( TextureUnit & unit )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		unit.setId( 0u );
		auto it = std::remove_if( m_dirty.begin()
			, m_dirty.end()
			, [&unit]( TextureUnit const * lookup )
			{
				return lookup == &unit;
			} );
		m_dirty.erase( it, m_dirty.end() );
	}

	void TextureConfigurationBuffer::update( ashes::CommandBuffer const & commandBuffer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< TextureUnit const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin()
				, end
				, [this]( TextureUnit const * unit )
				{
					CU_Require( unit->getId() > 0 );
					auto & config = unit->getConfiguration();
					auto index = unit->getId() - 1u;

					auto & data = m_data[index];
					data.colour = texcfgbuf::writeFlags( config.colourMask );
					data.opacity = texcfgbuf::writeFlags( config.opacityMask );
					data.specular = texcfgbuf::writeFlags( config.specularMask );
					data.glossiness = texcfgbuf::writeFlags( config.glossinessMask );
					data.metalness = texcfgbuf::writeFlags( config.metalnessMask );
					data.roughness = texcfgbuf::writeFlags( config.roughnessMask );
					data.emissive = texcfgbuf::writeFlags( config.emissiveMask );
					data.occlusion = texcfgbuf::writeFlags( config.occlusionMask );
					data.transmittance = texcfgbuf::writeFlags( config.transmittanceMask );
					data.normal = texcfgbuf::writeFlags( config.normalMask );
					data.normalFactors = texcfgbuf::writeFlags( config.normalFactor, config.normalGMultiplier );
					data.height = texcfgbuf::writeFlags( config.heightMask );
					data.heightFactors = texcfgbuf::writeFlags( config.heightFactor );

					data.invAnim = texcfgbuf::writeFlags( config.needsYInversion, texcfgbuf::writeBool( unit->isTransformAnimated() ) );
					data.tileAnimSet = texcfgbuf::writeFlags( texcfgbuf::writeBool( unit->isTileAnimated() ), unit->getTexcoordSet() );
					data.translate = config.transform.translate;
					data.rotate = { config.transform.rotate.cos(), config.transform.rotate.sin(), 0.0f, 0.0f };
					data.scale = config.transform.scale;
					data.tileSet = castor::Point4f{ config.tileSet };
				} );

			m_buffer.setCount( uint32_t( m_configurations.size() ) );
			m_buffer.upload( commandBuffer );
		}
	}

	VkDescriptorSetLayoutBinding TextureConfigurationBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
	}

	ashes::WriteDescriptorSet TextureConfigurationBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	void TextureConfigurationBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}

	RenderDevice const & TextureConfigurationBuffer::getDevice()const
	{
		return m_buffer.getDevice();
	}
}
