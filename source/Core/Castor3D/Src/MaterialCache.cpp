#include "MaterialCache.hpp"

#include "Engine.hpp"
#include "SamplerCache.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Scene/SceneFileParser.hpp"
#include "Texture/Sampler.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	const String CacheTraits< Material, String >::Name = cuT( "Material" );

	void MaterialCache::Initialise()
	{
		auto l_lock = make_unique_lock( m_elements );

		if ( !m_elements.has( Material::DefaultMaterialName ) )
		{
			m_defaultMaterial = m_produce( Material::DefaultMaterialName );
			m_defaultMaterial->CreatePass();
			m_defaultMaterial->GetPass( 0 )->SetTwoSided( true );
		}
		else
		{
			m_defaultMaterial = m_elements.find( Material::DefaultMaterialName );
			GetEngine()->PostEvent( MakeInitialiseEvent( *m_defaultMaterial ) );
		}
	}

	void MaterialCache::Clear()
	{
		auto l_lock = make_unique_lock( m_elements );
		m_defaultMaterial.reset();
		m_elements.clear ();
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

		bool l_return = true;
		auto l_it = m_elements.begin();
		bool l_first = true;

		while ( l_return && l_it != m_elements.end() )
		{
			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_return = Material::TextWriter( String() )( * l_it->second, p_file );
			++l_it;
		}

		return l_return;
	}

	bool MaterialCache::Read( TextFile & p_file )
	{
		auto l_lock = make_unique_lock( m_elements );
		SceneFileParser l_parser( *GetEngine() );
		l_parser.ParseFile( p_file );
		return true;
	}
}
