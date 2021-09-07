#include "Castor3D/Cache/MaterialCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

CU_ImplementCUSmartPtr( castor3d, MaterialCache )

namespace castor3d
{
	const castor::String ResourceCacheTraitsT< castor3d::Material, castor::String >::Name = cuT( "Material" );
}

namespace castor
{
	using namespace castor3d;

	ResourceCacheT< Material, String, MaterialCacheTraits >::ResourceCacheT( Engine & engine )
		: ElementCacheT{ engine.getLogger()
			, [this]( ElementT & resource )
			{
				m_engine.postEvent( makeCpuInitialiseEvent( resource ) );
			}
			, [this]( ElementT & resource )
			{
				m_engine.postEvent( makeCpuCleanupEvent( resource ) );
			}
			, castor::ResourceMergerT< MaterialCache >{ "_" } }
		, m_engine{ engine }
	{
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::initialise( RenderDevice const & device
		, PassTypeID passType )
	{
		if ( !m_passBuffer )
		{
			auto lock( makeUniqueLock( *this ) );
			MaterialResPtr created;
			auto defaultMaterial = doTryAddNoLockT( Material::DefaultMaterialName
				, false
				, created
				, m_engine, passType );
			auto material = defaultMaterial.lock();

			if ( created.lock() == material )
			{
				material->createPass();
				material->getPass( 0 )->setTwoSided( true );
			}

			m_defaultMaterial = material.get();
			m_passBuffer = std::make_shared< PassBuffer >( m_engine
				, device
				, shader::MaxMaterialsCount );
			m_textureBuffer = std::make_shared< TextureConfigurationBuffer >( m_engine
				, device
				, shader::MaxTextureConfigurationCount );

			for ( auto & it : *this )
			{
				if ( !it.second->isInitialised() )
				{
					it.second->initialise();
				}
			}
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::cleanup()
	{
		{
			auto lock( makeUniqueLock( *this ) );
			doCleanupNoLock();
		}
		m_engine.postEvent( makeCpuFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_passBuffer.reset();
				m_textureBuffer.reset();
			} ) );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::clear()
	{
		auto lock( makeUniqueLock( *this ) );
		m_defaultMaterial = {};
		doClearNoLock();
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( makeUniqueLock( *this ) );

		for ( auto & it : *this )
		{
			for ( auto & pass : *it.second )
			{
				pass->update();
			}
		}

	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::update( GpuUpdater & updater )
	{
		if ( m_passBuffer )
		{
			m_passBuffer->update();
			m_textureBuffer->update();
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::getNames( StringArray & names )
	{
		auto lock( makeUniqueLock( *this ) );
		names.clear();
		auto it = begin();

		while ( it != end() )
		{
			names.push_back( it->first );
			it++;
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::registerPass( Pass & pass )
	{
		if ( m_passBuffer )
		{
			m_passBuffer->addPass( pass );
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::unregisterPass( Pass & pass )
	{
		if ( m_passBuffer
			&& pass.getId() )
		{
			m_passBuffer->removePass( pass );
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::registerUnit( TextureUnit & unit )
	{
		if ( m_textureBuffer )
		{
			m_textureBuffer->addTextureConfiguration( unit );
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::unregisterUnit( TextureUnit & unit )
	{
		if ( m_textureBuffer
			&& unit.getId() )
		{
			m_textureBuffer->removeTextureConfiguration( unit );
		}
	}
}
