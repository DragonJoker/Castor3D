#include "Castor3D/Cache/LightCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>

CU_ImplementSmartPtr( c3d, LightCache )

namespace c3d
{
	namespace cachelgt
	{
		static String const C3D_UniqueDirectionalLight = cuT( "Only one directional light is allowed." );
	}

	const String ObjectCacheTraitsT< Light, String >::Name = cuT( "Light" );

	ObjectCacheT< Light, String, LightCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeRPtr rootNode
		, SceneNodeRPtr rootCameraNode
		, SceneNodeRPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootObjectNode
			, [this]( ElementT & element )
			{
				doRegisterLight( element );
			}
			, [this]( ElementT const & element )
			{
				doUnregisterLight( element );
			}
			, MovableMergerT< LightCache >{ scene.getName() }
			, MovableAttacherT< LightCache >{}
			, MovableDetacherT< LightCache >{} }
	{
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_lightBuffer )
		{
			auto lock( makeUniqueLock( *this ) );
			m_lightBuffer = makeUnique< LightBuffer >( device, getScene()->getResources(), MaxLightsCount );
			Vector< Light * > pending;
			c3d::swap( pending, m_pendingLights );

			for ( auto light : pending )
			{
				doRegisterLight( *light );
			}
		}
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::cleanup()
	{
		{
			auto lock( makeUniqueLock( *this ) );
			doCleanupNoLock();
		}
		m_engine.postEvent( makeCpuFunctorEvent( CpuEventType::ePreGpuStep
			, [this]()
			{
				m_lightBuffer.reset();
			} ) );
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( makeUniqueLock( *this ) );
		auto & sceneObjs = updater.dirtyScenes[getScene()];
		Vector< LightInstance * > dirty;
		dirty.insert( dirty.end()
			, sceneObjs.dirtyLights.begin()
			, sceneObjs.dirtyLights.end() );

		if ( !dirty.empty() )
		{
			m_dirty = true;
			auto end = std::unique( dirty.begin(), dirty.end() );
			dirty.erase( end, dirty.end() );

			for ( auto light : dirty )
			{
				light->update( updater );
			}
		}

		if ( m_lightBuffer )
		{
			m_lightBuffer->update( updater );
			m_dirty = !dirty.empty();
		}
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::upload( UploadData & uploader )const
	{
		if ( m_lightBuffer )
		{
			m_lightBuffer->upload( uploader );
		}
	}

	Vector< Light * > const & ObjectCacheT< Light, String, LightCacheTraits >::getLights( LightType type )const
	{
		return m_lightsPerType[size_t( type )];
	}

	LightInstancesArray ObjectCacheT< Light, String, LightCacheTraits >::getLightInstances( LightType type )const
	{
		return ( m_lightBuffer
			? m_lightBuffer->getLightInstances( type )
			: LightInstancesArray{} );
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::createPassBinding( crg::FramePass & pass
		, uint32_t binding )const
	{
		CU_Require( m_lightBuffer );
		m_lightBuffer->createPassBinding( pass, binding );
	}

	VkDescriptorSetLayoutBinding ObjectCacheT< Light, String, LightCacheTraits >::createLayoutBinding( VkShaderStageFlags stages
		, uint32_t index )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->createLayoutBinding( stages, index );
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::addLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags stages
			, uint32_t & index )const
	{
		bindings.emplace_back( createLayoutBinding( stages, index ) );
		++index;
	}

	ashes::WriteDescriptorSet ObjectCacheT< Light, String, LightCacheTraits >::getBinding( uint32_t binding )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->getBinding( binding );
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::addBinding( ashes::WriteDescriptorSetArray & writes
		, uint32_t & binding )const
	{
		writes.emplace_back( getBinding( binding ) );
		++binding;
	}

	ashes::WriteDescriptorSet ObjectCacheT< Light, String, LightCacheTraits >::getBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->getSingleBinding( binding, offset, size );
	}

	uint32_t ObjectCacheT< Light, String, LightCacheTraits >::getLightsBufferCount( LightType type )const noexcept
	{
		if ( m_lightBuffer )
		{
			return m_lightBuffer->getLightsBufferCount( type );
		}

		return 0u;
	}

	bool ObjectCacheT< Light, String, LightCacheTraits >::doCheckUniqueDirectionalLight( LightType toAdd )const noexcept
	{
		bool result = toAdd != LightType::eDirectional
			|| getLightsBufferCount( LightType::eDirectional ) == 0u;

		if ( !result )
		{
			log::error << cachelgt::C3D_UniqueDirectionalLight << std::endl;
		}

		return result;
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::doRegisterLight( Light & light )
	{
		if ( m_lightBuffer )
		{
			auto & typeLights = m_lightsPerType[size_t( light.getLightType() )];
			auto it = std::find_if( typeLights.begin(), typeLights.end()
				, [&light]( Light const * lookup )
				{
						return lookup->getName() == light.getName();
				} );

			if ( it == typeLights.end() )
			{
				typeLights.push_back( &light );
				m_lightBuffer->addLight( *light.getInstance() );
			}
		}
		else
		{
			m_pendingLights.push_back( &light );
		}
	}

	void ObjectCacheT< Light, String, LightCacheTraits >::doUnregisterLight( Light const & light )
	{
		auto & typeLights = m_lightsPerType[size_t( light.getLightType() )];
		auto it = std::find_if( typeLights.begin(), typeLights.end()
			, [&light]( Light const * lookup )
			{
				return lookup->getName() == light.getName();
			} );

		if ( it != typeLights.end() )
		{
			typeLights.erase( it );

			if ( m_lightBuffer )
			{
				m_lightBuffer->removeLight( *light.getInstance() );
			}
		}
	}
}
