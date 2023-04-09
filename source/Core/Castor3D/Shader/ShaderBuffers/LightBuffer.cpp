#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

CU_ImplementSmartPtr( castor3d, LightBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace lgtbuf
	{
		static LightBuffer::LightsData doBindData( uint8_t * buffer
			, uint32_t count )
		{
			return castor::makeArrayView( reinterpret_cast< castor::Point4f * >( buffer )
				, reinterpret_cast< castor::Point4f * >( buffer ) + count );
		}
	}

	//*********************************************************************************************

	LightBuffer::LightBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * 512u, cuT( "LightBuffer" ) }
		, m_lightSizes{ DirectionalLight::LightDataComponents
			, PointLight::LightDataComponents
			, SpotLight::LightDataComponents }
		, m_data{ lgtbuf::doBindData( m_buffer.getPtr(), count ) }
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

			// Mark next lights as dirty (due to sorted container)
			doMarkNextDirty( LightType( index + 1u ), 0u );
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
			auto dist = uint32_t( std::distance( m_typeSortedLights[index].begin(), it ) );
			m_typeSortedLights[index].erase( it );
			m_connections.erase( &light );

			// Mark next lights as dirty (due to sorted container)
			doMarkNextDirty( LightType( index + 1u ), dist );
		}
	}

	void LightBuffer::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( !m_dirty.empty() )
		{
			std::vector< Light * > dirty;
			std::swap( m_dirty, dirty );

			for ( auto light : castor::makeArrayView( dirty.begin(), std::unique( dirty.begin(), dirty.end() ) ) )
			{
				auto index = doGetOffset( *light );
				light->fillBuffer( index, &m_data[index] );
			}

			auto dirEnd = uint32_t( m_typeSortedLights[0].size() * DirectionalLight::LightDataComponents );
			auto pntEnd = uint32_t( dirEnd + m_typeSortedLights[1].size() * PointLight::LightDataComponents );
			auto sptEnd = uint32_t( pntEnd + m_typeSortedLights[2].size() * SpotLight::LightDataComponents );
			m_buffer.setFirstCount( dirEnd );
			m_buffer.setSecondCount( pntEnd );
			m_buffer.setThirdCount( sptEnd );
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

	ashes::WriteDescriptorSet LightBuffer::getSingleBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		return m_buffer.getSingleBinding( binding, offset, size );
	}

	uint32_t LightBuffer::doGetOffset( Light const & light )const
	{
		uint32_t result{};
		uint32_t index = uint32_t( light.getLightType() );

		for ( uint32_t i = 0u; i < index; ++i )
		{
			result += uint32_t( m_typeSortedLights[i].size() * m_lightSizes[i] );
		}

		auto it = std::find( m_typeSortedLights[index].begin()
			, m_typeSortedLights[index].end()
			, &light );
		auto size = m_typeSortedLights[index].size();

		if ( it != m_typeSortedLights[index].end() )
		{
			size = size_t( std::distance( m_typeSortedLights[index].begin(), it ) );
		}

		return result + uint32_t( size * m_lightSizes[index] );
	}

	void LightBuffer::doMarkNextDirty( LightType type
		, uint32_t index )
	{
		for ( auto i = uint32_t( type ); i < uint32_t( LightType::eMax ); ++i )
		{
			auto begin = m_typeSortedLights[i].begin() + index;

			for ( auto it : castor::makeArrayView( begin, m_typeSortedLights[i].end() ) )
			{
				m_dirty.emplace_back( it );
			}
		}
	}
}
