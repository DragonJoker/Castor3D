#include "MaterialManager.hpp"
#include "Material.hpp"
#include "Pass.hpp"
#include "SceneFileParser.hpp"
#include "Sampler.hpp"
#include "InitialiseEvent.hpp"
#include "CleanupEvent.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	MaterialManager::MaterialManager( Engine * p_pEngine )
		:	 m_pEngine( p_pEngine )
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
			m_defaultMaterial = std::make_shared< Material >( m_pEngine, Material::DefaultMaterialName );
			m_defaultMaterial->CreatePass();
			m_defaultMaterial->GetPass( 0 )->SetTwoSided( true );
			MaterialCollection::insert( Material::DefaultMaterialName, m_defaultMaterial );
		}

		m_pEngine->PostEvent( std::make_shared< InitialiseEvent< Material > >( *m_defaultMaterial ) );
	}

	void MaterialManager::Cleanup()
	{
		MaterialCollection::lock();

		std::for_each( begin(), end(), [&]( std::pair< String, MaterialSPtr > p_pair )
		{
			m_pEngine->PostEvent( std::make_shared< CleanupEvent< Material > >( *p_pair.second ) );
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
		m_pEngine->GetSamplerManager().lock();

		for ( SamplerCollection::TObjPtrMapIt l_it = m_pEngine->GetSamplerManager().begin(); l_it != m_pEngine->GetSamplerManager().end(); ++l_it )
		{
			Sampler::TextLoader()( *l_it->second, p_file );
		}

		m_pEngine->GetSamplerManager().unlock();
		MaterialCollection::lock();
		bool l_bReturn = true;
		MaterialCollectionConstIt l_it = begin();
		bool l_bFirst = true;

		while ( l_bReturn && l_it != end() )
		{
			if ( l_bFirst )
			{
				l_bFirst = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_bReturn = Material::TextLoader()( * l_it->second, p_file );
			++l_it;
		}

		MaterialCollection::unlock();
		return l_bReturn;
	}

	bool MaterialManager::Read( TextFile & p_file )
	{
		SceneFileParser l_parser( m_pEngine );
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
