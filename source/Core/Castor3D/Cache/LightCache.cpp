#include "Castor3D/Cache/LightCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
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

CU_ImplementSmartPtr( castor3d, LightCache )

namespace castor3d
{
	namespace cachelgt
	{
		static castor::String const C3D_UniqueDirectionalLight = cuT( "Only one directional light is allowed." );
	}

	const castor::String ObjectCacheTraitsT< Light, castor::String >::Name = cuT( "Light" );

	ObjectCacheT< Light, castor::String, LightCacheTraits >::ObjectCacheT( Scene & scene
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
			, [this]( ElementT & element )
			{
				doUnregisterLight( element );
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
				, MaxLightsCount );
			std::vector< Light * > pending;
			std::swap( pending, m_pendingLights );

			for ( auto light : pending )
			{
				doRegisterLight( *light );
			}

		}
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::cleanup()
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

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto & sceneObjs = updater.dirtyScenes[getScene()];
		LightsRefArray dirty;
		std::swap( m_dirtyLights, dirty );
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

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::upload( UploadData & uploader )const
	{
		if ( m_lightBuffer )
		{
			m_lightBuffer->upload( uploader );
		}
	}

	LightsArray ObjectCacheT< Light, castor::String, LightCacheTraits >::getLights( LightType type )const
	{
		return ( m_lightBuffer
			? m_lightBuffer->getLights( type )
			: LightsArray{} );
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::createPassBinding( crg::FramePass & pass
		, uint32_t binding )const
	{
		CU_Require( m_lightBuffer );
		m_lightBuffer->createPassBinding( pass, binding );
	}

	VkDescriptorSetLayoutBinding ObjectCacheT< Light, castor::String, LightCacheTraits >::createLayoutBinding( VkShaderStageFlags stages
		, uint32_t index )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->createLayoutBinding( stages, index );
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::addLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags stages
			, uint32_t & index )const
	{
		bindings.emplace_back( createLayoutBinding( stages, index ) );
		++index;
	}

	ashes::WriteDescriptorSet ObjectCacheT< Light, castor::String, LightCacheTraits >::getBinding( uint32_t binding )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->getBinding( binding );
	}

	void ObjectCacheT< Light, castor::String, LightCacheTraits >::addBinding( ashes::WriteDescriptorSetArray & writes
		, uint32_t & binding )const
	{
		writes.emplace_back( getBinding( binding ) );
		++binding;
	}

	ashes::WriteDescriptorSet ObjectCacheT< Light, castor::String, LightCacheTraits >::getBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		CU_Require( m_lightBuffer );
		return m_lightBuffer->getSingleBinding( binding, offset, size );
	}

	uint32_t ObjectCacheT< Light, castor::String, LightCacheTraits >::getLightsBufferCount( LightType type )const noexcept
	{
		if ( m_lightBuffer )
		{
			return m_lightBuffer->getLightsBufferCount( type );
		}

		return 0u;
	}

	bool ObjectCacheT< Light, castor::String, LightCacheTraits >::doCheckUniqueDirectionalLight( LightType toAdd )const noexcept
	{
		bool result = toAdd != LightType::eDirectional
			|| getLightsBufferCount( LightType::eDirectional ) == 0u;

		if ( !result )
		{
			log::error << cachelgt::C3D_UniqueDirectionalLight << std::endl;
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
