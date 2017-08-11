#include "MaterialCache.hpp"

#include "Engine.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Scene/SceneFileParser.hpp"
#include "Shader/LegacyPassBuffer.hpp"
#include "Shader/MetallicRoughnessPassBuffer.hpp"
#include "Shader/SpecularGlossinessPassBuffer.hpp"
#include "Texture/Sampler.hpp"

#include <GlslMaterial.hpp>

using namespace castor;

namespace castor3d
{
	template<> const String CacheTraits< Material, String >::Name = cuT( "Material" );

	void MaterialCache::initialise( MaterialType type )
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
			getEngine()->postEvent( MakeInitialiseEvent( *m_defaultMaterial ) );
		}

		switch ( type )
		{
		case MaterialType::eLegacy:
			m_passBuffer = std::make_shared< LegacyPassBuffer >( *getEngine(), glsl::MaxMaterialsCount );
			break;

		case MaterialType::ePbrMetallicRoughness:
			m_passBuffer = std::make_shared< MetallicRoughnessPassBuffer >( *getEngine(), glsl::MaxMaterialsCount );
			break;

		case MaterialType::ePbrSpecularGlossiness:
			m_passBuffer = std::make_shared< SpecularGlossinessPassBuffer >( *getEngine(), glsl::MaxMaterialsCount );
			break;
		}
	}

	void MaterialCache::cleanup()
	{
		m_passBuffer.reset();
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
				castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << name );
				result = m_elements.find( name );
			}
			else
			{
				m_elements.insert( name, element );
			}
		}
		else
		{
			castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_NULL_OBJECT << getObjectTypeName() << cuT( ": " ) );
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
			castor::Logger::logInfo( castor::StringStream() << INFO_CACHE_CREATED_OBJECT << getObjectTypeName() << cuT( ": " ) << name );
		}
		else
		{
			result = m_elements.find( name );
			castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << name );
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

	bool MaterialCache::write( TextFile & file )const
	{
		auto lockA = makeUniqueLock( m_elements );
		{
			auto lockB = makeUniqueLock( getEngine()->getSamplerCache() );
			getEngine()->getSamplerCache().lock();

			for ( auto it : getEngine()->getSamplerCache() )
			{
				Sampler::TextWriter( String{} )( *it.second, file );
			}
		}

		bool result = true;
		auto it = m_elements.begin();
		bool first = true;

		while ( result && it != m_elements.end() )
		{
			if ( first )
			{
				first = false;
			}
			else
			{
				file.writeText( cuT( "\n" ) );
			}

			result = Material::TextWriter( String() )( * it->second, file );
			++it;
		}

		return result;
	}

	bool MaterialCache::read( TextFile & file )
	{
		auto lock = makeUniqueLock( m_elements );
		SceneFileParser parser( *getEngine() );
		parser.parseFile( file );
		return true;
	}
}
