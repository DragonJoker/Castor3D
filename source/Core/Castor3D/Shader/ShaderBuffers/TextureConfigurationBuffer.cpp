#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

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
			auto colOpa = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto glsShn = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto emsOcc = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto trsDum = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto nmlFcr = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto hgtFcr = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			buffer += sizeof( castor::Point4f ) * count;
			auto mscVls = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
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
			buffer += sizeof( castor::Point4f ) * count;
			auto tileSet = makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
			return
			{
				colOpa,
				glsShn,
				emsOcc,
				trsDum,
				nmlFcr,
				hgtFcr,
				mscVls,
				translate,
				rotate,
				scale,
				tileSet,
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

		float writeBool( bool v )
		{
			return v ? 1.0f : 0.0f;
		}
	}

	//*********************************************************************************************

	TextureConfigurationBuffer::TextureConfigurationBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "TextureConfigurationBuffer" ) }
		, m_data{ doBindData( m_buffer.getPtr(), m_buffer.getSize(), count ) }
	{
	}

	uint32_t TextureConfigurationBuffer::addTextureConfiguration( TextureUnit & unit )
	{
		if ( unit.getId() == 0u )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

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

#if C3D_TextureConfigStructOfArrays

					m_data.colOpa[index] = writeFlags( config.colourMask, config.opacityMask );
					m_data.spcShn[index] = writeFlags( config.specularMask, config.glossinessMask );
					m_data.metRgh[index] = writeFlags( config.metalnessMask, config.roughnessMask );
					m_data.emsOcc[index] = writeFlags( config.emissiveMask, config.occlusionMask );
					m_data.trsDum[index] = writeFlags( config.transmittanceMask, {} );
					m_data.nmlFcr[index] = writeFlags( config.normalMask, config.normalFactor, config.normalGMultiplier );
					m_data.hgtFcr[index] = writeFlags( config.heightMask, config.heightFactor );
					m_data.mscVls[index] = writeFlags( float( config.needsYInversion ), writeBool( unit->isTransformAnimated() ), writeBool( unit->isTileAnimated() ) );
					m_data.data.translate[index] = config.translate;
					m_data.data.rotate[index] = config.rotate;
					m_data.data.scale[index] = config.scale;
					m_data.data.tileSet[index] = castor::Point4f{ config.tileSet };

#else

					auto & data = m_data[index];
					data.colOpa = writeFlags( config.colourMask, config.opacityMask );
					data.spcShn = writeFlags( config.specularMask, config.glossinessMask );
					data.metRgh = writeFlags( config.metalnessMask, config.roughnessMask );
					data.emsOcc = writeFlags( config.emissiveMask, config.occlusionMask );
					data.trsDum = writeFlags( config.transmittanceMask, {} );
					data.nmlFcr = writeFlags( config.normalMask, config.normalFactor, config.normalGMultiplier );
					data.hgtFcr = writeFlags( config.heightMask, config.heightFactor );
					data.mscVls = writeFlags( float( config.needsYInversion ), writeBool( unit->isTransformAnimated() ), writeBool( unit->isTileAnimated() ) );
					data.translate = config.transform.translate;
					data.rotate = { config.transform.rotate.cos(), config.transform.rotate.sin(), 0.0f, 0.0f };
					data.scale = config.transform.scale;
					data.tileSet = castor::Point4f{ config.tileSet };

#endif
				} );

			m_buffer.upload( commandBuffer );
		}
	}

	VkDescriptorSetLayoutBinding TextureConfigurationBuffer::createLayoutBinding( uint32_t binding )const
	{
		return m_buffer.createLayoutBinding( binding );
	}

	void TextureConfigurationBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}
}
