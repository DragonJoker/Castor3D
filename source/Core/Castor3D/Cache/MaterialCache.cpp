#include "Castor3D/Cache/MaterialCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass.hpp"
#include "Castor3D/Shader/PassBuffer/LegacyPassBuffer.hpp"
#include "Castor3D/Shader/PassBuffer/MetallicRoughnessPassBuffer.hpp"
#include "Castor3D/Shader/PassBuffer/SpecularGlossinessPassBuffer.hpp"
#include "Castor3D/Texture/Sampler.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

namespace castor3d
{
	template<> const String CacheTraits< Material, String >::Name = cuT( "Material" );

	void MaterialCache::initialise( MaterialType type )
	{
		if ( !m_passBuffer )
		{
			auto lock = makeUniqueLock( m_elements );

			if ( !m_elements.has( Material::DefaultMaterialName ) )
			{
				m_defaultMaterial = m_produce( Material::DefaultMaterialName, type );
				m_defaultMaterial->createPass();
				m_defaultMaterial->getPass( 0 )->setTwoSided( true );
			}
			else
			{
				m_defaultMaterial = m_elements.find( Material::DefaultMaterialName );
				getEngine()->postEvent( makeInitialiseEvent( *m_defaultMaterial ) );
			}

			switch ( type )
			{
			case MaterialType::eLegacy:
				m_passBuffer = std::make_shared< LegacyPassBuffer >( *getEngine(), shader::MaxMaterialsCount );
				break;

			case MaterialType::ePbrMetallicRoughness:
				m_passBuffer = std::make_shared< MetallicRoughnessPassBuffer >( *getEngine(), shader::MaxMaterialsCount );
				break;

			case MaterialType::ePbrSpecularGlossiness:
				m_passBuffer = std::make_shared< SpecularGlossinessPassBuffer >( *getEngine(), shader::MaxMaterialsCount );
				break;
			}
		}
	}

	void MaterialCache::cleanup()
	{
		getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_passBuffer.reset();
			} ) );
		auto lock = castor::makeUniqueLock( m_elements );

		for ( auto it : m_elements )
		{
			m_clean( it.second );
		}
	}

	void MaterialCache::update()
	{
		if ( m_passBuffer )
		{
			auto lock = makeUniqueLock( m_elements );

			for ( auto & material : m_elements )
			{
				for ( auto & pass : *material.second )
				{
					if ( pass->getId() == 0 )
					{
						m_passBuffer->addPass( *pass );
					}
				}
			}

			m_passBuffer->update();
		}
	}

	void MaterialCache::clear()
	{
		auto lock = makeUniqueLock( m_elements );
		m_defaultMaterial.reset();
		m_elements.clear();
	}

	MaterialSPtr MaterialCache::add( Key const & name, MaterialSPtr element )
	{
		MaterialSPtr result{ element };

		if ( element )
		{
			auto lock = castor::makeUniqueLock( m_elements );

			if ( m_elements.has( name ) )
			{
				result = m_elements.find( name );
				doReportDuplicate( name );
			}
			else
			{
				m_elements.insert( name, element );
			}
		}
		else
		{
			doReportNull();
		}

		for ( auto & pass : *result )
		{
			if ( pass->getId() == 0 )
			{
				m_passBuffer->addPass( *pass );
			}
		}

		return result;
	}

	MaterialSPtr MaterialCache::add( Key const & name, MaterialType type )
	{
		MaterialSPtr result;
		auto lock = castor::makeUniqueLock( m_elements );

		if ( !m_elements.has( name ) )
		{
			result = m_produce( name, type );
			m_initialise( result );
			m_elements.insert( name, result );
			doReportCreation( name );
		}
		else
		{
			result = m_elements.find( name );
			doReportDuplicate( name );
		}

		for ( auto & pass : *result )
		{
			if ( pass->getId() == 0 )
			{
				m_passBuffer->addPass( *pass );
			}
		}

		return result;
	}

	void MaterialCache::getNames( StringArray & names )
	{
		auto lock = makeUniqueLock( m_elements );
		names.clear();
		auto it = m_elements.begin();

		while ( it != m_elements.end() )
		{
			names.push_back( it->first );
			it++;
		}
	}
}
