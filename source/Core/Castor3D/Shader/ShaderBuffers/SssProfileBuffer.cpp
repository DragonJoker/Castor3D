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
		: m_buffer{ device, count * DataSize, cuT( "SssProfileBuffer" ) }
		, m_diffusionProfiles{ device
			, engine.getGraphResourceCache()
			, cuT( "DiffusionProfiles" )
			, 0u
			, { 512u, 1u, 1u }
			, uint32_t( ashes::getAlignedSize( count, 64u ) ) /*layerCount*/
			, 1u /*mipLevels*/
			, castor::PixelFormat::eR16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, FilterMode::eLinear
			, FilterMode::eLinear
			, MipmapMode::eLinear }
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
			auto lock( castor::makeUniqueLock( m_mutex ) );

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

	void SssProfileBuffer::update( UploadData & uploader )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			castor::Vector< SubsurfaceScatteringComponent const * > dirty;
			castor::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			for ( auto component : castor::makeArrayView( dirty.begin(), end ) )
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

	VkDescriptorSetLayoutBinding SssProfileBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
	}

	void SssProfileBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, binding );
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
		result.transmittanceProfileSize = &data.transmittanceProfileSize;
		result.gaussianWidth = &data.gaussianWidth;
		result.subsurfaceScatteringStrength = &data.subsurfaceScatteringStrength;
		result.transmittanceProfile = &data.transmittanceProfile;
		result.thicknessScale = &data.thicknessScale;

		return result;
	}
}
