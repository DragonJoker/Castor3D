#include "MaterialManager.hpp"
#include "Material.hpp"
#include "Pass.hpp"
#include "SceneFileParser.hpp"
#include "Sampler.hpp"
#include "InitialiseEvent.hpp"
#include "CleanupEvent.hpp"
#include "Engine.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	MaterialManager::MaterialManager( Engine * p_engine )
		:	 m_engine( p_engine )
	{
	}

	MaterialManager::~MaterialManager()
	{
		m_defaultMaterial.reset();
	}

	void MaterialManager::Initialise()
	{
		m_defaultMaterial = MaterialCollection::find( Material::DefaultMaterialName );

		if ( !m_defaultMaterial )
		{
			m_defaultMaterial = std::make_shared< Material >( m_engine, Material::DefaultMaterialName );
			m_defaultMaterial->CreatePass();
			m_defaultMaterial->GetPass( 0 )->SetTwoSided( true );
			MaterialCollection::insert( Material::DefaultMaterialName, m_defaultMaterial );
		}

		m_engine->PostEvent( MakeInitialiseEvent( *m_defaultMaterial ) );
	}

	void MaterialManager::Cleanup()
	{
		MaterialCollection::lock();

		std::for_each( begin(), end(), [&]( std::pair< String, MaterialSPtr > p_pair )
		{
			m_engine->PostEvent( MakeCleanupEvent( *p_pair.second ) );
		} );

		MaterialCollection::unlock();
	}

	void MaterialManager::DeleteAll()
	{
		m_defaultMaterial.reset();
		MaterialCollection::clear();
	}

	void MaterialManager::GetNames( StringArray & l_names )
	{
		MaterialCollection::lock();
		l_names.clear();
		MaterialCollectionConstIt l_it = begin();

		while ( l_it != end() )
		{
			l_names.push_back( l_it->first );
			l_it++;
		}

		MaterialCollection::unlock();
	}

	bool MaterialManager::Write( TextFile & p_file )const
	{
		m_engine->GetSamplerManager().lock();

		for ( SamplerCollection::TObjPtrMapIt l_it = m_engine->GetSamplerManager().begin(); l_it != m_engine->GetSamplerManager().end(); ++l_it )
		{
			Sampler::TextLoader()( *l_it->second, p_file );
		}

		m_engine->GetSamplerManager().unlock();
		MaterialCollection::lock();
		bool l_return = true;
		MaterialCollectionConstIt l_it = begin();
		bool l_first = true;

		while ( l_return && l_it != end() )
		{
			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_return = Material::TextLoader()( * l_it->second, p_file );
			++l_it;
		}

		MaterialCollection::unlock();
		return l_return;
	}

	bool MaterialManager::Read( TextFile & p_file )
	{
		SceneFileParser l_parser( m_engine );
		l_parser.ParseFile( p_file );
		return true;
	}

	bool MaterialManager::Save( BinaryFile & p_file )const
	{
		return false;
	}

	bool MaterialManager::Load( BinaryFile & p_file )
	{
		return false;
	}
}
