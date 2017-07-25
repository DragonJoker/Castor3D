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

using namespace Castor;

namespace Castor3D
{
	template<> const String CacheTraits< Material, String >::Name = cuT( "Material" );

	void MaterialCache::Initialise( MaterialType type )
	{
		auto lock = make_unique_lock( m_elements );

		if ( !m_elements.has( Material::DefaultMaterialName ) )
		{
			m_defaultMaterial = m_produce( Material::DefaultMaterialName, type );
			m_defaultMaterial->CreatePass();
			m_defaultMaterial->GetPass( 0 )->SetTwoSided( true );
		}
		else
		{
			m_defaultMaterial = m_elements.find( Material::DefaultMaterialName );
			GetEngine()->PostEvent( MakeInitialiseEvent( *m_defaultMaterial ) );
		}

		switch ( type )
		{
		case MaterialType::eLegacy:
			m_passBuffer = std::make_shared< LegacyPassBuffer >( *GetEngine(), GLSL::MaxMaterialsCount );
			break;

		case MaterialType::ePbrMetallicRoughness:
			m_passBuffer = std::make_shared< MetallicRoughnessPassBuffer >( *GetEngine(), GLSL::MaxMaterialsCount );
			break;

		case MaterialType::ePbrSpecularGlossiness:
			m_passBuffer = std::make_shared< SpecularGlossinessPassBuffer >( *GetEngine(), GLSL::MaxMaterialsCount );
			break;
		}
	}

	void MaterialCache::Cleanup()
	{
		m_passBuffer.reset();
		auto lock = Castor::make_unique_lock( m_elements );

		for ( auto it : m_elements )
		{
			m_clean( it.second );
		}
	}

	void MaterialCache::Update()
	{
		if ( m_passBuffer )
		{
			auto lock = make_unique_lock( m_elements );

			for ( auto & material : m_elements )
			{
				for ( auto & pass : *material.second )
				{
					if ( pass->GetId() == 0 )
					{
						m_passBuffer->AddPass( *pass );
					}
				}
			}

			m_passBuffer->Update();
		}
	}

	void MaterialCache::Clear()
	{
		auto lock = make_unique_lock( m_elements );
		m_defaultMaterial.reset();
		m_elements.clear();
	}

	MaterialSPtr MaterialCache::Add( Key const & name, MaterialSPtr element )
	{
		MaterialSPtr result{ element };

		if ( element )
		{
			auto lock = Castor::make_unique_lock( m_elements );

			if ( m_elements.has( name ) )
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << GetObjectTypeName() << cuT( ": " ) << name );
				result = m_elements.find( name );
			}
			else
			{
				m_elements.insert( name, element );
			}
		}
		else
		{
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_NULL_OBJECT << GetObjectTypeName() << cuT( ": " ) );
		}

		for ( auto & pass : *result )
		{
			if ( pass->GetId() == 0 )
			{
				m_passBuffer->AddPass( *pass );
			}
		}

		return result;
	}

	MaterialSPtr MaterialCache::Add( Key const & name, MaterialType type )
	{
		MaterialSPtr result;
		auto lock = Castor::make_unique_lock( m_elements );

		if ( !m_elements.has( name ) )
		{
			result = m_produce( name, type );
			m_initialise( result );
			m_elements.insert( name, result );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << GetObjectTypeName() << cuT( ": " ) << name );
		}
		else
		{
			result = m_elements.find( name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << GetObjectTypeName() << cuT( ": " ) << name );
		}

		for ( auto & pass : *result )
		{
			if ( pass->GetId() == 0 )
			{
				m_passBuffer->AddPass( *pass );
			}
		}

		return result;
	}

	void MaterialCache::GetNames( StringArray & names )
	{
		auto lock = make_unique_lock( m_elements );
		names.clear();
		auto it = m_elements.begin();

		while ( it != m_elements.end() )
		{
			names.push_back( it->first );
			it++;
		}
	}

	bool MaterialCache::Write( TextFile & file )const
	{
		auto lockA = make_unique_lock( m_elements );
		{
			auto lockB = make_unique_lock( GetEngine()->GetSamplerCache() );
			GetEngine()->GetSamplerCache().lock();

			for ( auto it : GetEngine()->GetSamplerCache() )
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
				file.WriteText( cuT( "\n" ) );
			}

			result = Material::TextWriter( String() )( * it->second, file );
			++it;
		}

		return result;
	}

	bool MaterialCache::Read( TextFile & file )
	{
		auto lock = make_unique_lock( m_elements );
		SceneFileParser parser( *GetEngine() );
		parser.ParseFile( file );
		return true;
	}
}
