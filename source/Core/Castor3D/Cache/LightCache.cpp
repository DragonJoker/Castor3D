#include "Castor3D/Cache/LightCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>

CU_ImplementCUSmartPtr( castor3d, LightCache )

namespace castor3d
{
	namespace
	{
		static castor::String const C3D_UniqueDirectionalLight = cuT( "Only one directional light is allowed." );
	}

	const castor::String ObjectCacheTraitsT< Light, castor::String >::Name = cuT( "Light" );

	ObjectCacheT< Light, castor::String, LightCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootObjectNode
			, [this]( ElementT & element )
			{
				getScene()->registerLight( element );
				m_dirtyLights.emplace_back( &element );
				m_connections.emplace( &element
					, element.onChanged.connect( [this]( Light & light )
						{
							onLightChanged( light );
						} ) );
				auto index = size_t( element.getLightType() );
				auto it = std::find( m_typeSortedLights[index].begin()
					, m_typeSortedLights[index].end()
					, &element );

				if ( it == m_typeSortedLights[index].end() )
				{
					m_typeSortedLights[index].push_back( &element );
				}
			}
			, [this]( ElementT & element )
			{
				auto index = size_t( element.getLightType() );
				auto it = std::find( m_typeSortedLights[index].begin()
					, m_typeSortedLights[index].end()
					, &element );

				if ( it != m_typeSortedLights[index].end() )
				{
					m_typeSortedLights[index].erase( it );
				}

				m_connections.erase( &element );
				getScene()->unregisterLight( element );
			}
			, MovableMergerT< LightCache >{ scene.getName() }
			, MovableAttacherT< LightCache >{}
			, MovableDetacherT< LightCache >{} }
	{
		auto & engine = *getScene()->getEngine();
		m_lightsBuffer.resize( 300ull * shader::getMaxLightComponentsCount() );
		m_textureBuffer = makeBuffer< castor::Point4f >( engine.getRenderSystem()->getRenderDevice()
			, uint32_t( m_lightsBuffer.size() )
			, ( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				| VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT
				| VK_BUFFER_USAGE_TRANSFER_DST_BIT )
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LightsBuffer" );
		m_textureView = ( engine.getRenderSystem()->getRenderDevice() )->createBufferView( "LightsBufferView"
			, m_textureBuffer->getBuffer()
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, 0u
			, uint32_t( m_lightsBuffer.size() * sizeof( castor::Point4f ) ) );
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::cleanup()
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_dirtyLights.clear();
		m_connections.clear();
		ElementObjectCacheT::doCleanupNoLock();
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );

		if ( !m_dirtyLights.empty() )
		{
			LightsRefArray dirty;
			std::swap( m_dirtyLights, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			for ( auto light : makeArrayView( dirty.begin(), end ) )
			{
				light->update( updater );
			}
		}
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::update( GpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto & camera = *updater.camera;
		uint32_t index = 0;
		uint32_t lightIndex = 0;
		castor::Point4f * data = m_lightsBuffer.data();

		for ( auto lights : m_typeSortedLights )
		{
			for ( auto light : lights )
			{
				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox()
						, light->getParent()->getDerivedTransformationMatrix() ) )
				{
					light->bind( lightIndex++, data );
					data += shader::getMaxLightComponentsCount();
					index += shader::getMaxLightComponentsCount();
				}
			}
		}

		if ( index )
		{
			if ( auto * locked = m_textureBuffer->lock( 0u
				, index
				, 0u ) )
			{
				std::copy( m_lightsBuffer.begin(), m_lightsBuffer.begin() + index, locked );
				m_textureBuffer->flush( 0u, index );
				m_textureBuffer->unlock();
			}
		}
	}

	ashes::WriteDescriptorSet ObjectCacheT< Light, castor::String, LightCacheTraits >::getDescriptorWrite( uint32_t binding )const
	{
		auto & buffer = getBuffer().getBuffer();
		auto & view = getView();
		ashes::WriteDescriptorSet write{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER };
		write.bufferInfo.push_back( { buffer, view.getOffset(), view.getRange() } );
		write.texelBufferView.push_back( view );
		return write;
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::onLightChanged( Light & light )
	{
		m_dirtyLights.emplace_back( &light );
	}

	bool ObjectCacheT< Light, castor::String, LightCacheTraits >::doCheckUniqueDirectionalLight( LightType toAdd )
	{
		bool result = toAdd != LightType::eDirectional
			|| getLightsCount( LightType::eDirectional ) == 0u;

		if ( !result )
		{
			log::error << C3D_UniqueDirectionalLight << std::endl;
		}

		return result;
	}
}
