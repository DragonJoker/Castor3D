#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
#if C3D_MaterialsStructOfArrays

		PassBuffer::PassesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			auto colourDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto specDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto edgeFactors = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto edgeColour = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto specific = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto common = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto opacity = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto reflRefr = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto sssInfo = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( buffer )
				, reinterpret_cast< PassBuffer::RgbaColour * >( buffer ) + count );
			buffer += sizeof( PassBuffer::RgbaColour ) * count;
			auto transmittance = makeArrayView( reinterpret_cast< std::array< PassBuffer::RgbaColour, 10u > * >( buffer )
				, reinterpret_cast< std::array< PassBuffer::RgbaColour, 10u > * >( buffer ) + count );
			return
			{
				colourDiv,
				specDiv,
				edgeFactors,
				edgeColour,
				specific,
				common,
				opacity,
				reflRefr,
				sssInfo,
				transmittance,
			};
		}

#else

		PassBuffer::PassesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			return castor::makeArrayView( reinterpret_cast< PassBuffer::PassData * >( buffer )
				, reinterpret_cast< PassBuffer::PassData * >( buffer ) + count );
		}

#endif
	}

	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "PassBuffer" ) }
		, m_data{ doBindData( m_buffer.getPtr(), count ) }
	{
	}

	uint32_t PassBuffer::addPass( Pass & pass )
	{
		if ( pass.getId() == 0u )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			CU_Require( m_passes.size() < shader::MaxMaterialsCount );
			m_passes.emplace_back( &pass );
			pass.setId( m_passID++ );
			m_connections.emplace_back( pass.onChanged.connect( [this]( Pass const & ppass )
				{
					m_dirty.emplace_back( &ppass );
				} ) );
			m_dirty.emplace_back( &pass );
		}

		return pass.getId();
	}

	void PassBuffer::removePass( Pass & pass )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		auto id = pass.getId() - 1u;
		CU_Require( id < m_passes.size() );
		CU_Require( &pass == m_passes[id] );
		auto it = m_passes.erase( m_passes.begin() + id );
		m_connections.erase( m_connections.begin() + id );
		++id;

		while ( it != m_passes.end() )
		{
			( *it )->setId( id );
			++it;
			++id;
		}

		pass.setId( 0u );
		m_passID--;
	}

	void PassBuffer::update()
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< Pass const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin(), end, [this]( Pass const * p_pass )
			{
				p_pass->fillBuffer( *this );
			} );

			m_buffer.update();
		}
	}

	VkDescriptorSetLayoutBinding PassBuffer::createLayoutBinding( uint32_t binding )const
	{
		return m_buffer.createLayoutBinding( binding );
	}

	void PassBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "Materials" ), binding );
	}

	void PassBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}

	ashes::WriteDescriptorSet PassBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	PassBuffer::PassDataPtr PassBuffer::getData( uint32_t passID )
	{
		CU_Require( passID > 0 );
		auto index = passID - 1;
		PassDataPtr result{};

#if C3D_MaterialsStructOfArrays

		result.colourDiv = &m_data.colourDiv[index];
		result.specDiv = &m_data.specDiv[index];
		result.edgeFactors = &data.edgeFactors[index];
		result.edgeColour = &data.edgeColour[index];
		result.specific = &data.specific[index];
		result.common = &data.common[index];
		result.opacity = &data.opacity[index];
		result.reflRefr = &m_data.reflRefr[index];
		result.extended.sssInfo = &data.extended.sssInfo[index];
		result.extended.transmittanceProfile = &data.extended.transmittanceProfile[index];


#else

		auto & data = m_data[index];
		result.colourDiv = &data.colourDiv;
		result.specDiv = &data.specDiv;
		result.edgeFactors = &data.edgeFactors;
		result.edgeColour = &data.edgeColour;
		result.specific = &data.specific;
		result.common = &data.common;
		result.opacity = &data.opacity;
		result.reflRefr = &data.reflRefr;
		result.extended.sssInfo = &data.extended.sssInfo;
		result.extended.transmittanceProfile = &data.extended.transmittanceProfile;

#endif

		return result;
	}
}
