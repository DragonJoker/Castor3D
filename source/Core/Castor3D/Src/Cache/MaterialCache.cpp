#include "MaterialCache.hpp"

#include "Engine.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Scene/SceneFileParser.hpp"
#include "Shader/PassBuffer.hpp"
#include "Texture/Sampler.hpp"

#include <GlslMaterial.hpp>

using namespace Castor;

namespace Castor3D
{
	template<> const String CacheTraits< Material, String >::Name = cuT( "Material" );

	void MaterialCache::Initialise()
	{
		auto l_lock = make_unique_lock( m_elements );

		if ( !m_elements.has( Material::DefaultMaterialName ) )
		{
			m_defaultMaterial = m_produce( Material::DefaultMaterialName, MaterialType::eLegacy );
			m_defaultMaterial->CreatePass();
			m_defaultMaterial->GetPass( 0 )->SetTwoSided( true );
		}
		else
		{
			m_defaultMaterial = m_elements.find( Material::DefaultMaterialName );
			GetEngine()->PostEvent( MakeInitialiseEvent( *m_defaultMaterial ) );
		}

		m_passBuffer = std::make_shared< PassBuffer >( *GetEngine(), GLSL::LegacyMaterials::Size );
	}

	void MaterialCache::Cleanup()
	{
		m_passBuffer.reset();
		auto l_lock = Castor::make_unique_lock( m_elements );

		for ( auto l_it : m_elements )
		{
			m_clean( l_it.second );
		}
	}

	void MaterialCache::Update()
	{
		if ( m_passBuffer )
		{
			auto l_lock = make_unique_lock( m_elements );

			for ( auto & l_material : m_elements )
			{
				for ( auto & l_pass : *l_material.second )
				{
					if ( l_pass->GetId() == 0 )
					{
						m_passBuffer->AddPass( *l_pass );
					}
				}
			}

			m_passBuffer->Update();
		}
	}

	void MaterialCache::Clear()
	{
		auto l_lock = make_unique_lock( m_elements );
		m_defaultMaterial.reset();
		m_elements.clear();
	}

	MaterialSPtr MaterialCache::Add( Key const & p_name, MaterialSPtr p_element )
	{
		MaterialSPtr l_result{ p_element };

		if ( p_element )
		{
			auto l_lock = Castor::make_unique_lock( m_elements );

			if ( m_elements.has( p_name ) )
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << GetObjectTypeName() << cuT( ": " ) << p_name );
				l_result = m_elements.find( p_name );
			}
			else
			{
				m_elements.insert( p_name, p_element );
			}
		}
		else
		{
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_NULL_OBJECT << GetObjectTypeName() << cuT( ": " ) );
		}

		for ( auto & l_pass : *l_result )
		{
			if ( l_pass->GetId() == 0 )
			{
				m_passBuffer->AddPass( *l_pass );
			}
		}

		return l_result;
	}

	MaterialSPtr MaterialCache::Add( Key const & p_name, MaterialType p_type )
	{
		MaterialSPtr l_result;
		auto l_lock = Castor::make_unique_lock( m_elements );

		if ( !m_elements.has( p_name ) )
		{
			l_result = m_produce( p_name, p_type );
			m_initialise( l_result );
			m_elements.insert( p_name, l_result );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << GetObjectTypeName() << cuT( ": " ) << p_name );
		}
		else
		{
			l_result = m_elements.find( p_name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << GetObjectTypeName() << cuT( ": " ) << p_name );
		}

		for ( auto & l_pass : *l_result )
		{
			if ( l_pass->GetId() == 0 )
			{
				m_passBuffer->AddPass( *l_pass );
			}
		}

		return l_result;
	}

	void MaterialCache::GetNames( StringArray & l_names )
	{
		auto l_lock = make_unique_lock( m_elements );
		l_names.clear();
		auto l_it = m_elements.begin();

		while ( l_it != m_elements.end() )
		{
			l_names.push_back( l_it->first );
			l_it++;
		}
	}

	bool MaterialCache::Write( TextFile & p_file )const
	{
		auto l_lockA = make_unique_lock( m_elements );
		{
			auto l_lockB = make_unique_lock( GetEngine()->GetSamplerCache() );
			GetEngine()->GetSamplerCache().lock();

			for ( auto l_it : GetEngine()->GetSamplerCache() )
			{
				Sampler::TextWriter( String{} )( *l_it.second, p_file );
			}
		}

		bool l_result = true;
		auto l_it = m_elements.begin();
		bool l_first = true;

		while ( l_result && l_it != m_elements.end() )
		{
			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_result = Material::TextWriter( String() )( * l_it->second, p_file );
			++l_it;
		}

		return l_result;
	}

	bool MaterialCache::Read( TextFile & p_file )
	{
		auto l_lock = make_unique_lock( m_elements );
		SceneFileParser l_parser( *GetEngine() );
		l_parser.ParseFile( p_file );
		return true;
	}
}
