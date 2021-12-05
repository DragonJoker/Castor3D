#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

CU_ImplementCUSmartPtr( castor3d, LightBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		LightBuffer::LightsData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			return castor::makeArrayView( reinterpret_cast< LightBuffer::LightData * >( buffer )
				, reinterpret_cast< LightBuffer::LightData * >( buffer ) + count );
		}
	}

	//*********************************************************************************************

	LightBuffer::LightBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "LightBuffer" ) }
		, m_data{ doBindData( m_buffer.getPtr(), count ) }
	{
	}

	void LightBuffer::addLight( Light & light )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto index = size_t( light.getLightType() );
		auto it = std::find( m_typeSortedLights[index].begin()
			, m_typeSortedLights[index].end()
			, &light );

		if ( it == m_typeSortedLights[index].end() )
		{
			m_typeSortedLights[index].push_back( &light );
			m_dirty.emplace_back( &light );
			m_connections.emplace( &light
				, light.onGPUChanged.connect( [this]( Light & plight )
				{
					m_dirty.emplace_back( &plight );
				} ) );
		}
	}

	void LightBuffer::removeLight( Light & light )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto index = size_t( light.getLightType() );
		auto it = std::find( m_typeSortedLights[index].begin()
			, m_typeSortedLights[index].end()
			, &light );

		if ( it != m_typeSortedLights[index].end() )
		{
			m_typeSortedLights[index].erase( it );
			m_connections.erase( &light );
		}
	}

	void LightBuffer::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< Light * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );
			std::for_each( dirty.begin()
				, end
				, [this]( Light * light )
				{
					auto index = doGetIndex( *light );
					light->fillBuffer( index, m_data[index] );
				} );
			m_wasDirty = true;
		}
	}

	void LightBuffer::upload( ashes::CommandBuffer const & commandBuffer )
	{
		if ( m_wasDirty )
		{
			m_buffer.upload( commandBuffer );
			m_wasDirty = false;
		}
	}

	VkDescriptorSetLayoutBinding LightBuffer::createLayoutBinding( uint32_t binding )const
	{
		return m_buffer.createLayoutBinding( binding );
	}

	void LightBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "Lights" ), binding );
	}

	void LightBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}

	ashes::WriteDescriptorSet LightBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	ashes::WriteDescriptorSet LightBuffer::getBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		return m_buffer.getBinding( binding, offset, size );
	}

	LightBuffer::LightData & LightBuffer::getData( Light const & light )
	{
		return m_data[getIndex( light )];
	}

	uint32_t LightBuffer::getIndex( Light const & light )const
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		return doGetIndex( light );
	}

	uint32_t LightBuffer::doGetIndex( Light const & light )const
	{
		uint32_t result{};
		uint32_t index = uint32_t( light.getLightType() );

		for ( uint32_t i = 0u; i < index; ++i )
		{
			result += uint32_t( m_typeSortedLights[i].size() );
		}

		auto it = std::find( m_typeSortedLights[index].begin()
			, m_typeSortedLights[index].end()
			, &light );

		if ( it != m_typeSortedLights[index].end() )
		{
			return result + uint32_t( std::distance( m_typeSortedLights[index].begin(), it ) );
		}

		return result + uint32_t( m_typeSortedLights[index].size() );
	}
}
