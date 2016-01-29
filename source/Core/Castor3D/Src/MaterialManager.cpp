#include "MaterialManager.hpp"
#include "Material.hpp"
#include "Pass.hpp"
#include "SceneFileParser.hpp"
#include "SamplerManager.hpp"
#include "InitialiseEvent.hpp"
#include "CleanupEvent.hpp"
#include "Engine.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	MaterialManager::MaterialManager( Engine & p_engine )
		: Manager< Castor::String, Material, Engine >( p_engine )
	{
	}

	MaterialManager::~MaterialManager()
	{
		m_defaultMaterial.reset();
	}

	void MaterialManager::Initialise()
	{
		std::unique_lock< Collection > l_lock( m_elements );

		if ( !m_elements.has( Material::DefaultMaterialName ) )
		{
			m_defaultMaterial = Create( Material::DefaultMaterialName, *GetOwner(), Material::DefaultMaterialName );
			m_defaultMaterial->CreatePass();
			m_defaultMaterial->GetPass( 0 )->SetTwoSided( true );
		}
		else
		{
			m_defaultMaterial = m_elements.find( Material::DefaultMaterialName );
			GetOwner()->PostEvent( MakeInitialiseEvent( *m_defaultMaterial ) );
		}
	}

	void MaterialManager::Clear()
	{
		std::unique_lock< Collection > l_lock( m_elements );
		m_defaultMaterial.reset();
		Manager< Castor::String, Material, Engine >::Clear();
	}

	void MaterialManager::GetNames( StringArray & l_names )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		l_names.clear();
		auto l_it = m_elements.begin();

		while ( l_it != m_elements.end() )
		{
			l_names.push_back( l_it->first );
			l_it++;
		}
	}

	bool MaterialManager::Write( TextFile & p_file )const
	{
		std::unique_lock< Collection > l_lock( m_elements );
		GetOwner()->GetSamplerManager().lock();

		for ( auto l_it : GetOwner()->GetSamplerManager() )
		{
			Sampler::TextLoader()( *l_it.second, p_file );
		}

		GetOwner()->GetSamplerManager().unlock();

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

			l_return = Material::TextLoader()( * l_it->second, p_file );
			++l_it;
		}

		return l_return;
	}

	bool MaterialManager::Read( TextFile & p_file )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		SceneFileParser l_parser( *GetOwner() );
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
