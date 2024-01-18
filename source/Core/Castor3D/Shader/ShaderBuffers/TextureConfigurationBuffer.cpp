#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

CU_ImplementSmartPtr( castor3d, TextureConfigurationBuffer )

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
	}

	//*********************************************************************************************

	TextureConfigurationBuffer::TextureConfigurationBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * VkDeviceSize( DataSize ), cuT( "TextureConfigurationBuffer" ) }
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

	void TextureConfigurationBuffer::removeTextureConfiguration( TextureUnit & unit )noexcept
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

	void TextureConfigurationBuffer::update( UploadData & uploader )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< TextureUnit const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			for ( auto unit : castor::makeArrayView( dirty.begin(), end ) )
			{
				CU_Require( unit->getId() > 0 );
				auto & config = unit->getConfiguration();
				auto index = unit->getId() - 1u;

				if ( index > m_data.size() )
				{
					log::warn << "TextureUnit [" << unit->getId() << "] is out of buffer boundaries, ignoring it." << std::endl;
				}
				else
				{
					auto & data = m_data[index];
					data.translate = config.transform.translate;
					data.rotateU = config.transform.rotate.cos();
					data.rotateV = config.transform.rotate.sin();
					data.scale = config.transform.scale;
					data.tileSet = castor::Point4f{ config.tileSet };
					data.normalFactor = config.normalFactor;
					data.normalGMult = config.normalDirectX ? -1.0f : 1.0f;
					data.normal2Chan = config.normal2Channels ? 1u : 0u;
					data.heightFactor = config.heightFactor;
					data.needsYInv = config.needsYInversion ? 1u : 0u;
					data.isTransformAnim = unit->isTransformAnimated() ? 1u : 0u;
					data.isTileAnim = unit->isTileAnimated() ? 1u : 0u;
					data.texcoordSet = unit->getTexcoordSet() ? 1u : 0u;
				}
			}

			m_buffer.setCount( uint32_t( std::min( m_data.size() * DataSize, m_configurations.size() ) ) );
			m_buffer.upload( uploader );
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
