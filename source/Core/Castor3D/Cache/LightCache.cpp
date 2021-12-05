#include "Castor3D/Cache/LightCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

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
							m_dirtyLights.emplace_back( &light );
						} ) );
				doRegisterLight( element );
			}
			, [this]( ElementT & element )
			{
				doUnregisterLight( element );
				m_connections.erase( &element );
				getScene()->unregisterLight( element );
			}
			, MovableMergerT< LightCache >{ scene.getName() }
			, MovableAttacherT< LightCache >{}
			, MovableDetacherT< LightCache >{} }
	{
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_lightBuffer )
		{
			auto lock( makeUniqueLock( *this ) );
			m_lightBuffer = castor::makeUnique< LightBuffer >( m_engine
				, device
				, shader::MaxLightsCount );

			for ( auto light : m_pendingLights )
			{
				doRegisterLight( *light );
			}

			m_pendingLights.clear();
		}
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::cleanup()
	{
		{
			auto lock( makeUniqueLock( *this ) );
			doCleanupNoLock();
		}
		m_engine.postEvent( makeCpuFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_lightBuffer.reset();
			} ) );
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

		if ( m_lightBuffer )
		{
			m_lightBuffer->update( updater );
		}
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::update( GpuUpdater & updater )
	{
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::upload( ashes::CommandBuffer const & cb )const
	{
		if ( m_lightBuffer )
		{
			m_lightBuffer->upload( cb );
		}
	}

	LightsArray ObjectCacheT< Light, castor::String, LightCacheTraits >::getLights( LightType type )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->getLights( type );
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::createPassBinding( crg::FramePass & pass
		, uint32_t binding )const
	{
		CU_Require( m_lightBuffer );
		m_lightBuffer->createPassBinding( pass, binding );
	}

	VkDescriptorSetLayoutBinding ObjectCacheT< Light, castor::String, LightCacheTraits >::createLayoutBinding( uint32_t index )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->createLayoutBinding( index );
	}

	ashes::WriteDescriptorSet ObjectCacheT< Light, castor::String, LightCacheTraits >::getBinding( uint32_t binding )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->getBinding( binding );
	}

	ashes::WriteDescriptorSet ObjectCacheT< Light, castor::String, LightCacheTraits >::getBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->getBinding( binding, offset, size );
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

	bool ObjectCacheT< Light, castor::String, LightCacheTraits >::doRegisterLight( Light & light )
	{
		if ( m_lightBuffer )
		{
			m_lightBuffer->addLight( light );
			return true;
		}

		m_pendingLights.push_back( &light );
		return false;
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::doUnregisterLight( Light & light )
	{
		if ( m_lightBuffer )
		{
			m_lightBuffer->removeLight( light );
		}
	}
}
