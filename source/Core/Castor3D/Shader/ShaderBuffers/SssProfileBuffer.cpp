#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"

CU_ImplementSmartPtr( c3d, SssProfileBuffer )

namespace c3d
{
	//*********************************************************************************************

	namespace sssbuf
	{
		static SssProfileBuffer::SssProfilesData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			return makeArrayView( reinterpret_cast< SssProfileBuffer::SssProfileData * >( buffer )
				, reinterpret_cast< SssProfileBuffer::SssProfileData * >( buffer ) + count );
		}
	}

	//*********************************************************************************************

	SssProfileBuffer::SssProfileBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: ShaderBufferHolder{ device, engine.getGraphResourceCache(), count * DataSize, cuT( "SssProfileBuffer" ) }
		, m_diffusionProfiles{ device
			, engine.getGraphResourceCache()
			, cuT( "DiffusionProfiles" )
			, { ImageCreateFlags::eNone
				, { 512u, 1u, 1u }, uint32_t( ashes::getAlignedSize( count, 64u ) ), 1u
				, PixelFormat::eR16G16B16A16_SFLOAT
				, ImageUsageFlags::eStorage | ImageUsageFlags::eSampled }
			, {} }
		, m_data{ sssbuf::doBindData( m_buffer.getPtr(), count ) }
	{
		m_diffusionProfiles.create();
	}

	SssProfileBuffer::~SssProfileBuffer()noexcept
	{
		m_diffusionProfiles.destroy();
	}

	uint32_t SssProfileBuffer::addPass( SubsurfaceScatteringComponent & component )
	{
		if ( component.getSssProfileId() == 0u )
		{
			auto lock( makeUniqueLock( m_mutex ) );

			CU_Require( m_components.size() < MaxMaterialsCount );
			m_components.emplace_back( &component );
			component.setSssProfileId( m_profileID );
			++m_profileID;
			m_connections.emplace_back( component.onProfileChanged.connect( [this]( SubsurfaceScatteringComponent const & comp )
				{
					m_dirty.emplace_back( &comp );
				} ) );
			m_dirty.emplace_back( &component );
		}

		return component.getSssProfileId();
	}

	void SssProfileBuffer::removePass( SubsurfaceScatteringComponent & component )noexcept
	{
		auto lock( makeUniqueLock( m_mutex ) );

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

	void SssProfileBuffer::update( UploadData & uploader )
	{
		auto lock( makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			Vector< SubsurfaceScatteringComponent const * > dirty;
			c3d::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			for ( auto component : makeArrayView( dirty.begin(), end ) )
			{
				if ( component->getSssProfileId() > m_data.size() )
				{
					log::warn << "SssProfile [" << component->getSssProfileId() << "] is out of buffer boundaries, ignoring it." << std::endl;
				}
				else
				{
					component->fillProfileBuffer( *this );
				}
			}

			m_buffer.setCount( uint32_t( std::min( m_data.size(), m_components.size() ) ) );
			m_buffer.upload( uploader );
		}
	}

	SssProfileBuffer::SssProfileDataPtr SssProfileBuffer::getData( uint32_t profileID )
	{
		CU_Require( profileID > 0 );
		auto index = profileID - 1;
		SssProfileDataPtr result{};

		auto & data = m_data[index];
		result.transmittanceProfileSize = &data.transmittanceProfileSize;
		result.gaussianWidth = &data.gaussianWidth;
		result.subsurfaceScatteringStrength = &data.subsurfaceScatteringStrength;
		result.transmittanceProfile = &data.transmittanceProfile;
		result.thicknessScale = &data.thicknessScale;

		return result;
	}
}
