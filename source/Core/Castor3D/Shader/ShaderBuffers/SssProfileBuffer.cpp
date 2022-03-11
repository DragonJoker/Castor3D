#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

CU_ImplementCUSmartPtr( castor3d, SssProfileBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		SssProfileBuffer::SssProfilesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			return castor::makeArrayView( reinterpret_cast< SssProfileBuffer::SssProfileData * >( buffer )
				, reinterpret_cast< SssProfileBuffer::SssProfileData * >( buffer ) + count );
		}
	}

	//*********************************************************************************************

	SssProfileBuffer::SssProfileBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "SssProfileBuffer" ) }
		, m_data{ doBindData( m_buffer.getPtr(), count ) }
	{
	}

	uint32_t SssProfileBuffer::addPass( Pass & pass )
	{
		if ( pass.getSssProfileId() == 0u )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			CU_Require( m_passes.size() < shader::MaxMaterialsCount );
			m_passes.emplace_back( &pass );
			pass.setSssProfileId( m_profileID++ );
			m_connections.emplace_back( pass.onSssProfileChanged.connect( [this]( Pass const & ppass )
				{
					m_dirty.emplace_back( &ppass );
				} ) );
			m_dirty.emplace_back( &pass );
		}

		return pass.getSssProfileId();
	}

	void SssProfileBuffer::removePass( Pass & pass )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		auto id = pass.getSssProfileId() - 1u;
		CU_Require( id < m_passes.size() );
		CU_Require( &pass == m_passes[id] );
		auto it = m_passes.erase( m_passes.begin() + id );
		m_connections.erase( m_connections.begin() + id );
		++id;

		while ( it != m_passes.end() )
		{
			( *it )->setSssProfileId( id );
			++it;
			++id;
		}

		pass.setSssProfileId( 0u );
		m_profileID--;
	}

	void SssProfileBuffer::update( ashes::CommandBuffer const & commandBuffer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< Pass const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin(), end, [this]( Pass const * p_pass )
			{
				p_pass->fillSssProfileBuffer( *this );
			} );

			m_buffer.upload( commandBuffer );
		}
	}

	VkDescriptorSetLayoutBinding SssProfileBuffer::createLayoutBinding( uint32_t binding )const
	{
		return m_buffer.createLayoutBinding( binding );
	}

	void SssProfileBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "SssProfiles" ), binding );
	}

	void SssProfileBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}

	ashes::WriteDescriptorSet SssProfileBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	SssProfileBuffer::SssProfileDataPtr SssProfileBuffer::getData( uint32_t profileID )
	{
		CU_Require( profileID > 0 );
		auto index = profileID - 1;
		SssProfileDataPtr result{};

		auto & data = m_data[index];
		result.sssInfo = &data.sssInfo;
		result.transmittanceProfile = &data.transmittanceProfile;

		return result;
	}
}
