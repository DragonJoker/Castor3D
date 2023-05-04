#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

CU_ImplementSmartPtr( castor3d, SssProfileBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace sssbuf
	{
		static SssProfileBuffer::SssProfilesData doBindData( uint8_t * buffer
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
		, m_data{ sssbuf::doBindData( m_buffer.getPtr(), count ) }
	{
	}

	uint32_t SssProfileBuffer::addPass( SubsurfaceScatteringComponent & component )
	{
		if ( component.getSssProfileId() == 0u )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			CU_Require( m_components.size() < MaxMaterialsCount );
			m_components.emplace_back( &component );
			component.setSssProfileId( m_profileID++ );
			m_connections.emplace_back( component.onProfileChanged.connect( [this]( SubsurfaceScatteringComponent const & comp )
				{
					m_dirty.emplace_back( &comp );
				} ) );
			m_dirty.emplace_back( &component );
		}

		return component.getSssProfileId();
	}

	void SssProfileBuffer::removePass( SubsurfaceScatteringComponent & component )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		auto id = component.getSssProfileId() - 1u;
		CU_Require( id < m_components.size() );
		CU_Require( &component == m_components[id] );
		auto it = m_components.erase( m_components.begin() + id );
		m_connections.erase( m_connections.begin() + id );
		++id;

		while ( it != m_components.end() )
		{
			( *it )->setSssProfileId( id );
			++it;
			++id;
		}

		component.setSssProfileId( 0u );
		m_profileID--;
	}

	void SssProfileBuffer::update( ashes::CommandBuffer const & commandBuffer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< SubsurfaceScatteringComponent const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin(), end, [this]( SubsurfaceScatteringComponent const * component )
			{
					component->fillProfileBuffer( *this );
			} );

			m_buffer.setCount( uint32_t( m_components.size() ) );
			m_buffer.upload( commandBuffer );
		}
	}

	VkDescriptorSetLayoutBinding SssProfileBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
	}

	void SssProfileBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "C3D_SssProfiles" ), binding );
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
