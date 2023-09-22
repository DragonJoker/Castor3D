#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Math/Math.hpp>

CU_ImplementSmartPtr( castor3d, LightBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace lgtbuf
	{
		static VkDeviceSize MaxLightComponentsCount = castor::maxValueT< uint32_t
			, DirectionalLight::LightDataComponents
			, PointLight::LightDataComponents
			, SpotLight::LightDataComponents >;
	}

	//*********************************************************************************************

	LightBuffer::LightBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine
			, device
			, VkDeviceSize( count ) * lgtbuf::MaxLightComponentsCount * sizeof( castor::Point4f )
			, cuT( "C3D_LightBuffer" ) }
		, m_lightSizes{ DirectionalLight::LightDataComponents
			, PointLight::LightDataComponents
			, SpotLight::LightDataComponents }
		, m_data{ castor::makeArrayView( reinterpret_cast< castor::Point4f * >( m_buffer.getPtr() )
			, VkDeviceSize( count ) * lgtbuf::MaxLightComponentsCount ) }
	{
	}

	void LightBuffer::addLight( Light & light )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto index = size_t( light.getLightType() );
		auto & lights = m_typeSortedLights[index];
		auto it = std::find( lights.begin()
			, lights.end()
			, &light );

		if ( it == lights.end() )
		{
			lights.push_back( &light );
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
		auto & lights = m_typeSortedLights[index];
		auto it = std::find( lights.begin()
			, lights.end()
			, &light );

		if ( it != lights.end() )
		{
			auto dist = uint32_t( std::distance( lights.begin(), it ) );
			lights.erase( it );
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
				auto [index, offset] = doGetOffsetIndex( *light );

				if ( index <= MaxLightsCount )
				{
					light->fillLightBuffer( index, offset, &m_data[offset] );
				}
			}

			m_buffer.setFirstCount( std::min( uint32_t( m_data.size() ), doGetBufferEnd( LightType::eDirectional ) ) );
			m_buffer.setSecondCount( std::min( uint32_t( m_data.size() ), doGetBufferEnd( LightType::ePoint ) ) );
			m_buffer.setThirdCount( std::min( uint32_t( m_data.size() ), doGetBufferEnd( LightType::eSpot ) ) );
			m_wasDirty = true;
		}
	}

	void LightBuffer::upload( UploadData & uploader )
	{
		if ( m_wasDirty )
		{
			m_buffer.upload( uploader );
			m_wasDirty = false;
		}
	}

	VkDescriptorSetLayoutBinding LightBuffer::createLayoutBinding( VkShaderStageFlags stages
		, uint32_t binding )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
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

	uint32_t LightBuffer::getLightsBufferCount( LightType lightType )const noexcept
	{
		uint32_t result{};
		auto type = uint32_t( lightType );

		for ( uint32_t i = 0u; i < type; ++i )
		{
			result += getLightsBufferCount( LightType( i ) );
		}

		auto count = uint32_t( m_typeSortedLights[type].size() );
		return ( ( result + count <= MaxLightsCount )
			? count
			: ( MaxLightsCount - result ) );
	}

	std::pair< uint32_t, uint32_t > LightBuffer::doGetOffsetIndex( Light const & light )const
	{
		uint32_t index{};
		uint32_t result{};
		uint32_t type = uint32_t( light.getLightType() );

		for ( uint32_t i = 0u; i < type; ++i )
		{
			auto count = getLightsBufferCount( LightType( i ) );
			index += count;
			result += count * m_lightSizes[i];
		}

		auto typeSortedLights = m_typeSortedLights[type];
		auto it = std::find( typeSortedLights.begin()
			, typeSortedLights.end()
			, &light );
		auto count = typeSortedLights.size();

		if ( it != typeSortedLights.end() )
		{
			count = size_t( std::distance( typeSortedLights.begin(), it ) );
		}

		return { uint32_t( index + count )
			, result + uint32_t( count * m_lightSizes[type] ) };
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

	uint32_t LightBuffer::doGetBufferEnd( LightType lightType )const noexcept
	{
		uint32_t bufferEnd{};
		auto type = uint32_t( lightType );

		for ( uint32_t i = 0u; i < type; ++i )
		{
			auto count = getLightsBufferCount( LightType( i ) );
			bufferEnd += count * m_lightSizes[i];
		}

		auto count = getLightsBufferCount( lightType );
		bufferEnd += count * m_lightSizes[type];
		return bufferEnd;
	}
}
