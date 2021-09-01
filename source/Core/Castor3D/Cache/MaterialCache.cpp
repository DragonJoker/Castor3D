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

namespace castor
{
	using namespace castor3d;

	ResourceCacheT< Material, String >::ResourceCacheT( Engine & engine )
		: ElementCacheT{ engine.getLogger()
			, [&engine]( String const & name, PassTypeID type )
			{
				return std::make_shared< Material >( name, engine, type );
			}
			, [this]( ElementPtrT resource )
			{
				m_engine.postEvent( makeCpuFunctorEvent( EventType::ePreRender
					, [this, resource]()
					{
						resource->initialise();
						registerMaterial( *resource );
					} ) );
			}
			, [this]( ElementPtrT resource )
			{
				m_engine.postEvent( makeCpuFunctorEvent( EventType::ePreRender
					, [this, resource]()
					{
						unregisterMaterial( *resource );
						resource->cleanup();
					} ) );
			} }
		, m_engine{ engine }
	{
	}

	void ResourceCacheT< Material, String >::initialise( RenderDevice const & device
		, PassTypeID passType )
	{
		if ( !m_passBuffer )
		{
			auto lock( makeUniqueLock( *this ) );
			m_defaultMaterial = doTryFindNoLock( Material::DefaultMaterialName );

			if ( !m_defaultMaterial )
			{
				m_defaultMaterial = m_produce( Material::DefaultMaterialName, passType );
				m_defaultMaterial->createPass();
				m_defaultMaterial->getPass( 0 )->setTwoSided( true );
			}

			m_passBuffer = std::make_shared< PassBuffer >( m_engine
				, device
				, shader::MaxMaterialsCount );
			m_textureBuffer = std::make_shared< TextureConfigurationBuffer >( m_engine
				, device
				, shader::MaxTextureConfigurationCount );

			for ( auto it : *this )
			{
				if ( !it.second->isInitialised() )
				{
					it.second->initialise();
				}
			}
		}
	}

	void ResourceCacheT< Material, String >::cleanup()
	{
		auto lock( makeUniqueLock( *this ) );
		doCleanupNoLock();
		m_engine.postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_passBuffer.reset();
				m_textureBuffer.reset();
			} ) );
	}

	void ResourceCacheT< Material, String >::clear()
	{
		auto lock( makeUniqueLock( *this ) );
		m_defaultMaterial.reset();
		doClearNoLock();
	}

	void ResourceCacheT< Material, String >::update( CpuUpdater & updater )
	{
		auto lock( makeUniqueLock( *this ) );

		for ( auto it : *this )
		{
			for ( auto & pass : *it.second )
			{
				pass->update();
			}
		}

	}

	void ResourceCacheT< Material, String >::update( GpuUpdater & updater )
	{
		auto lock( makeUniqueLock( *this ) );

		if ( m_passBuffer )
		{
			m_passBuffer->update();
			m_textureBuffer->update();
		}
	}

	void ResourceCacheT< Material, String >::getNames( StringArray & names )
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

	void ResourceCacheT< Material, String >::registerMaterial( Material const & material )
	{
		auto lock( makeUniqueLock( *this ) );

		for ( auto & pass : material )
		{
			if ( pass->getId() == 0 )
			{
				m_passBuffer->addPass( *pass );

				for ( auto & unit : *pass )
				{
					if ( unit->getId() == 0u )
					{
						m_textureBuffer->addTextureConfiguration( *unit );
					}
				}
			}
		}
	}

	void ResourceCacheT< Material, String >::unregisterMaterial( Material const & material )
	{
		auto lock( makeUniqueLock( *this ) );

		if ( m_passBuffer )
		{
			for ( auto & pass : material )
			{
				if ( pass->getId() != 0 )
				{
					for ( auto & unit : *pass )
					{
						if ( unit->getId() != 0u )
						{
							m_textureBuffer->removeTextureConfiguration( *unit );
						}
					}

					m_passBuffer->removePass( *pass );
				}
			}
		}
	}

	void ResourceCacheT< Material, String >::registerPass( Pass & pass )
	{
		auto lock( makeUniqueLock( *this ) );

		if ( m_passBuffer )
		{
			m_passBuffer->addPass( pass );
		}
	}

	void ResourceCacheT< Material, String >::unregisterPass( Pass & pass )
	{
		auto lock( makeUniqueLock( *this ) );

		if ( m_passBuffer )
		{
			m_passBuffer->removePass( pass );
		}
	}

	void ResourceCacheT< Material, String >::registerUnit( TextureUnit & unit )
	{
		auto lock( makeUniqueLock( *this ) );

		if ( m_textureBuffer )
		{
			m_textureBuffer->addTextureConfiguration( unit );
		}
	}

	void ResourceCacheT< Material, String >::unregisterUnit( TextureUnit & unit )
	{
		auto lock( makeUniqueLock( *this ) );

		if ( m_textureBuffer )
		{
			m_textureBuffer->removeTextureConfiguration( unit );
		}
	}
}
