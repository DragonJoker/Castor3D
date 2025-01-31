#include "Castor3D/Cache/LightGroupCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/LightGroup.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>

CU_ImplementSmartPtr( castor3d, LightGroupCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< LightGroup, castor::String >::Name = cuT( "LightGroup" );
}

namespace castor
{
	using namespace castor3d;

	ResourceCacheT< LightGroup, String, LightGroupCacheTraits >::ResourceCacheT( Scene & scene )
		: ElementCacheT{ scene.getEngine()->getLogger()
			, [this]( ElementT & element )
			{
				doRegisterLightGroup( element );
			}
			, [this]( ElementT & element )
			{
				doUnregisterLightGroup( element );
			} }
		, m_scene{ scene }
	{
	}

	void ResourceCacheT< LightGroup, String, LightGroupCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_lightBuffer )
		{
			auto lock( makeUniqueLock( *this ) );
			m_lightBuffer = &m_scene.getLightCache().getLightBuffer();
			castor::Vector< LightGroup * > pending;
			castor::swap( pending, m_pendingLights );

			for ( auto light : pending )
			{
				doRegisterLightGroup( *light );
			}
		}
	}

	void ResourceCacheT< LightGroup, String, LightGroupCacheTraits >::cleanup()
	{
		auto lock( makeUniqueLock( *this ) );
		doCleanupNoLock();
	}

	bool ResourceCacheT< LightGroup, String, LightGroupCacheTraits >::doRegisterLightGroup( LightGroup & light )
	{
		if ( m_lightBuffer )
		{
			for ( auto & instance : light )
			{
				m_lightBuffer->addLight( *instance );
			}

			return true;
		}

		m_pendingLights.push_back( &light );
		return false;
	}

	void ResourceCacheT< LightGroup, String, LightGroupCacheTraits >::doUnregisterLightGroup( LightGroup & light )
	{
		if ( m_lightBuffer )
		{
			for ( auto & instance : light )
			{
				m_lightBuffer->removeLight( *instance );
			}
		}
	}
}
