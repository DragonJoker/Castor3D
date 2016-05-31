#include "Material.hpp"

#include "Pass.hpp"

#include "Scene/SceneFileParser.hpp"
#include "Texture/Sampler.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Material::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Material >{ p_tabs }
	{
	}

	bool Material::TextWriter::operator()( Material const & p_material, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( m_tabs + cuT( "material \"" ) + p_material.GetName() + cuT( "\"\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		}

		bool l_first = true;

		for ( auto && l_pass : p_material )
		{
			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_return = Pass::TextWriter( m_tabs + cuT( "\t" ) )( *l_pass, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*********************************************************************************************

	const Castor::String Material::DefaultMaterialName = cuT( "DefaultMaterial" );

	Material::Material( String const & p_name, Engine & p_engine )
		: Resource<Material>( p_name )
		, OwnedBy< Engine >( p_engine )
	{
	}

	Material::~Material()
	{
	}

	void Material::Initialise()
	{
		Logger::LogDebug( cuT( "Initialising material [" ) + GetName() + cuT( "]" ) );

		for ( auto && l_pass : m_passes )
		{
			l_pass->Initialise();
		}
	}

	void Material::Cleanup()
	{
		for ( auto && l_pass : m_passes )
		{
			l_pass->Cleanup();
		}
	}

	PassSPtr Material::CreatePass()
	{
		PassSPtr l_newPass = std::make_shared< Pass >( *GetEngine(), shared_from_this() );
		m_passes.push_back( l_newPass );
		return l_newPass;
	}

	const PassSPtr Material::GetPass( uint32_t p_index )const
	{
		REQUIRE( p_index < m_passes.size() );
		return m_passes[p_index];
	}

	PassSPtr Material::GetPass( uint32_t p_index )
	{
		REQUIRE( p_index < m_passes.size() );
		return m_passes[p_index];
	}

	void Material::DestroyPass( uint32_t p_index )
	{
		REQUIRE( p_index < m_passes.size() );
		m_passes.erase( m_passes.begin() + p_index );
	}

	bool Material::HasAlphaBlending()const
	{
		bool l_return = true;
		uint32_t l_count = GetPassCount();

		for ( uint32_t i = 0; i < l_count && l_return; i++ )
		{
			l_return = m_passes[i]->HasAlphaBlending();
		}

		return l_return;
	}
}
