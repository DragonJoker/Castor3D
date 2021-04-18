#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
#if C3D_TextureConfigStructOfArrays

		TextureConfigurationBuffer::TextureConfigurationsData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			auto colrSpec = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto glossOpa = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto emisOccl = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto trnsDumm = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto normalFc = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto heightFc = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto miscVals = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto translate = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto rotate = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto scale = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			return
			{
				colrSpec,
				glossOpa,
				emisOccl,
				trnsDumm,
				normalFc,
				heightFc,
				miscVals,
				translate,
				rotate,
				scale,
			};
		}

#else

		TextureConfigurationBuffer::TextureConfigurationsData doBindData( uint8_t * buffer
			, VkDeviceSize size
			, uint32_t count )
		{
			CU_Require( ( count * sizeof( TextureConfigurationBuffer::Data ) ) <= size );
			return makeArrayView( reinterpret_cast< TextureConfigurationBuffer::Data * >( buffer )
				, reinterpret_cast< TextureConfigurationBuffer::Data * >( buffer ) + count );
		}

#endif

		castor::Point4f writeFlags( castor::Point2ui const & a
			, castor::Point2ui const & b )
		{
			return castor::Point4f
			{
				a[0] ? 1.0f : 0.0f,
				float( a[1] ),
				b[0] ? 1.0f : 0.0f,
				float( b[1] ),
			};
		}

		castor::Point4f writeFlags( castor::Point2ui const & a
			, float b
			, float c = 0.0f )
		{
			return castor::Point4f
			{
				a[0] ? 1.0f : 0.0f,
				float( a[1] ),
				b,
				c,
			};
		}

		castor::Point4f writeFlags( float a
			, float b
			, float c = 0.0f
			, float d = 0.0f )
		{
			return castor::Point4f
			{
				a,
				b,
				c,
				d,
			};
		}
	}

	//*********************************************************************************************

	TextureConfigurationBuffer::TextureConfigurationBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "TextureConfigurationBuffer" ) }
		, m_configMaxCount{ count }
		, m_data{ doBindData( m_buffer.getPtr(), m_buffer.getSize(), count ) }
	{
	}

	uint32_t TextureConfigurationBuffer::addTextureConfiguration( TextureUnit & unit )
	{
		CU_Require( unit.getId() == 0u );
		CU_Require( m_configurations.size() < m_configMaxCount - 1u );
		m_configurations.emplace_back( &unit );
		unit.setId( m_configID++ );
		m_connections.emplace_back( unit.onChanged.connect( [this]( TextureUnit const & unit )
		{
			m_dirty.emplace_back( &unit );
		} ) );
		m_dirty.emplace_back( &unit );
		return unit.getId();
	}

	void TextureConfigurationBuffer::removeTextureConfiguration( TextureUnit & unit )
	{
		auto id = unit.getId();
		CU_Require( id < m_configurations.size() );
		CU_Require( &unit == m_configurations[id] );

		for ( auto it = m_configurations.erase( m_configurations.begin() + id ); it != m_configurations.end(); ++it )
		{
			( *it )->setId( id );
			++id;
		}

		m_connections.erase( m_connections.begin() + id );
		unit.setId( 0u );
		m_configID--;
	}

	void TextureConfigurationBuffer::update()
	{
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
					auto index = unit->getId() - 1u;
					auto & config = unit->getConfiguration();

#if C3D_TextureConfigStructOfArrays

					m_data.colrSpec[index] = writeFlags( config.colourMask, config.specularMask );
					m_data.glossOpa[index] = writeFlags( config.glossinessMask, config.opacityMask );
					m_data.emisOccl[index] = writeFlags( config.emissiveMask, config.occlusionMask );
					m_data.trnsDumm[index] = writeFlags( config.transmittanceMask, {} );
					m_data.normalFc[index] = writeFlags( config.normalMask, config.normalFactor, config.normalGMultiplier );
					m_data.heightFc[index] = writeFlags( config.heightMask, config.heightFactor );
					m_data.miscVals[index] = writeFlags( float( config.needsGammaCorrection )
						, float( config.needsYInversion ) );
					m_data.data.translate[index] = config.translate;
					m_data.data.rotate[index] = config.rotate;
					m_data.data.scale[index] = config.scale;

#else

					auto & data = m_data[index];
					data.colrSpec = writeFlags( config.colourMask, config.specularMask );
					data.glossOpa = writeFlags( config.glossinessMask, config.opacityMask );
					data.emisOccl = writeFlags( config.emissiveMask, config.occlusionMask );
					data.trnsDumm = writeFlags( config.transmittanceMask, {} );
					data.normalFc = writeFlags( config.normalMask, config.normalFactor, config.normalGMultiplier );
					data.heightFc = writeFlags( config.heightMask, config.heightFactor );
					data.miscVals = writeFlags( float( config.needsGammaCorrection )
						, float( config.needsYInversion ) );
					data.translate = config.transform.translate;
					data.rotate = { config.transform.rotate.cos(), config.transform.rotate.sin(), 0.0f, 0.0f };
					data.scale = config.transform.scale;

#endif
				} );

			m_buffer.update();
		}
	}

	VkDescriptorSetLayoutBinding TextureConfigurationBuffer::createLayoutBinding()const
	{
		return m_buffer.createLayoutBinding( getTexturesBufferIndex() );
	}

	void TextureConfigurationBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}
}
