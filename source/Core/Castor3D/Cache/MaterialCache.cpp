#include "Castor3D/Cache/MaterialCache.hpp"

#include "Castor3D/Engine.hpp"
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

using namespace castor;

namespace castor3d
{
	namespace
	{
		using LockType = std::unique_lock< castor::Collection< Material, castor::String > >;
	}

	void MaterialCache::initialise( RenderDevice const & device
		, PassTypeID passType )
	{
		if ( !m_passBuffer )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			if ( !m_elements.has( Material::DefaultMaterialName ) )
			{
				m_defaultMaterial = m_produce( Material::DefaultMaterialName, passType );
				m_defaultMaterial->createPass();
				m_defaultMaterial->getPass( 0 )->setTwoSided( true );
			}
			else
			{
				m_defaultMaterial = m_elements.find( Material::DefaultMaterialName );
				m_defaultMaterial->initialise();
			}

			m_passBuffer = std::make_shared< PassBuffer >( *getEngine()
				, device
				, shader::MaxMaterialsCount );
			m_textureBuffer = std::make_shared< TextureConfigurationBuffer >( *getEngine()
				, device
				, shader::MaxTextureConfigurationCount );

			for ( auto it : m_elements )
			{
				if ( !it.second->isInitialised() )
				{
					it.second->initialise();
				}
			}
		}
	}

	void MaterialCache::cleanup()
	{
		getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_passBuffer.reset();
				m_textureBuffer.reset();
			} ) );
		auto lock( castor::makeUniqueLock( m_elements ) );

		for ( auto it : m_elements )
		{
			m_clean( it.second );
		}
	}

	void MaterialCache::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( m_elements ) );

		for ( auto it : m_elements )
		{
			for ( auto & pass : *it.second )
			{
				pass->update();
			}
		}

	}

	void MaterialCache::update( GpuUpdater & updater )
	{
		if ( m_passBuffer )
		{
			m_passBuffer->update();
			m_textureBuffer->update();
		}
	}

	void MaterialCache::clear()
	{
		LockType lock{ castor::makeUniqueLock( m_elements ) };
		m_defaultMaterial.reset();
		m_elements.clear();
	}

	bool MaterialCache::tryAdd( Key const & name
		, MaterialSPtr element
		, bool initialise )
	{
		bool result = false;

		if ( element )
		{
			result = ( element == m_elements.tryInsert( name, element ) );

			if ( result && initialise )
			{
				m_initialise( element );
			}
		}

		return result;
	}

	MaterialSPtr MaterialCache::add( Key const & name
		, MaterialSPtr element
		, bool initialise )
	{
		MaterialSPtr result{ element };

		if ( element )
		{
			result = m_elements.tryInsert( name, element );

			if ( result != element )
			{
				doReportDuplicate( name );
			}
			else if ( initialise )
			{
				m_initialise( element );
			}
		}
		else
		{
			doReportNull();
		}

		return result;
	}

	MaterialSPtr MaterialCache::add( Key const & name
		, PassTypeID type )
	{
		MaterialSPtr result;
		auto lock( castor::makeUniqueLock( m_elements ) );

		if ( !m_elements.has( name ) )
		{
			result = create( name, type );
			m_initialise( result );
			m_elements.insert( name, result );
			doReportCreation( name );
		}
		else
		{
			result = m_elements.find( name );
			doReportDuplicate( name );
		}

		return result;
	}

	void MaterialCache::remove( Key const & name )
	{
		m_elements.erase( name );
	}

	void MaterialCache::getNames( StringArray & names )
	{
		LockType lock{ castor::makeUniqueLock( m_elements ) };
		names.clear();
		auto it = m_elements.begin();

		while ( it != m_elements.end() )
		{
			names.push_back( it->first );
			it++;
		}
	}

	void MaterialCache::registerMaterial( Material const & material )
	{
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

	void MaterialCache::unregisterMaterial( Material const & material )
	{
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

	void MaterialCache::registerPass( Pass & pass )
	{
		if ( m_passBuffer )
		{
			m_passBuffer->addPass( pass );
		}
	}

	void MaterialCache::unregisterPass( Pass & pass )
	{
		if ( m_passBuffer )
		{
			m_passBuffer->removePass( pass );
		}
	}

	void MaterialCache::registerUnit( TextureUnit & unit )
	{
		if ( m_textureBuffer )
		{
			m_textureBuffer->addTextureConfiguration( unit );
		}
	}

	void MaterialCache::unregisterUnit( TextureUnit & unit )
	{
		if ( m_textureBuffer )
		{
			m_textureBuffer->removeTextureConfiguration( unit );
		}
	}
}
