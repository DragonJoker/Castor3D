#include "Castor3D/Cache/MaterialCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Shader/PassBuffer/PhongPassBuffer.hpp"
#include "Castor3D/Shader/PassBuffer/MetallicRoughnessPassBuffer.hpp"
#include "Castor3D/Shader/PassBuffer/SpecularGlossinessPassBuffer.hpp"
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

	template<> const String CacheTraits< Material, String >::Name = cuT( "Material" );

	void MaterialCache::initialise( MaterialType type )
	{
		if ( !m_passBuffer )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

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
			case MaterialType::ePhong:
				m_passBuffer = std::make_shared< PhongPassBuffer >( *getEngine(), shader::MaxMaterialsCount );
				break;

			case MaterialType::eMetallicRoughness:
				m_passBuffer = std::make_shared< MetallicRoughnessPassBuffer >( *getEngine(), shader::MaxMaterialsCount );
				break;

			case MaterialType::eSpecularGlossiness:
				m_passBuffer = std::make_shared< SpecularGlossinessPassBuffer >( *getEngine(), shader::MaxMaterialsCount );
				break;

			default:
				break;
			}

			m_textureBuffer = std::make_shared< TextureConfigurationBuffer >( *getEngine(), shader::MaxTextureConfigurationCount );
		}
	}

	void MaterialCache::cleanup()
	{
		getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
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

	void MaterialCache::update()
	{
		if ( m_passBuffer )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			for ( auto & material : m_elements )
			{
				doAddMaterial( *material.second );
			}

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

	MaterialSPtr MaterialCache::add( Key const & name, MaterialSPtr element )
	{
		MaterialSPtr result{ element };

		if ( element )
		{
			auto lock( castor::makeUniqueLock( m_elements ) );

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

		doAddMaterial( *result );
		return result;
	}

	MaterialSPtr MaterialCache::add( Key const & name, MaterialType type )
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

		doAddMaterial( *result );
		return result;
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

	void MaterialCache::doAddMaterial( Material const & material )
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
}
