﻿#include "Material.hpp"

#include "LegacyPass.hpp"
#include "MetallicRoughnessPbrPass.hpp"
#include "SpecularGlossinessPbrPass.hpp"

#include "Scene/SceneFileParser.hpp"

using namespace Castor;

namespace Castor3D
{
	Material::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Material >{ p_tabs }
	{
	}

	bool Material::TextWriter::operator()( Material const & p_material, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing Material " ) + p_material.GetName() );
		bool l_result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "material \"" ) + p_material.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< Material >::CheckError( l_result, "Material name" );

		if ( l_result )
		{
			switch ( p_material.GetType() )
			{
			case MaterialType::eLegacy:
				for ( auto l_pass : p_material )
				{
					l_result &= LegacyPass::TextWriter( m_tabs + cuT( "\t" ) )( *std::static_pointer_cast< LegacyPass >( l_pass ), p_file );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				for ( auto l_pass : p_material )
				{
					l_result &= MetallicRoughnessPbrPass::TextWriter( m_tabs + cuT( "\t" ) )( *std::static_pointer_cast< MetallicRoughnessPbrPass >( l_pass ), p_file );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				for ( auto l_pass : p_material )
				{
					l_result &= SpecularGlossinessPbrPass::TextWriter( m_tabs + cuT( "\t" ) )( *std::static_pointer_cast< SpecularGlossinessPbrPass >( l_pass ), p_file );
				}
				break;

			default:
				FAILURE( cuT( "Unsupported pass type" ) );
				break;
			}
		}

		if ( l_result )
		{
			l_result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_result;
	}

	//*********************************************************************************************

	const Castor::String Material::DefaultMaterialName = cuT( "DefaultMaterial" );

	Material::Material( String const & p_name, Engine & p_engine, MaterialType p_type )
		: Resource< Material >( p_name )
		, OwnedBy< Engine >( p_engine )
		, m_type{ p_type }
	{
	}

	Material::~Material()
	{
	}

	void Material::Initialise()
	{
		Logger::LogDebug( cuT( "Initialising material [" ) + GetName() + cuT( "]" ) );

		for ( auto l_pass : m_passes )
		{
			l_pass->Initialise();
		}
	}

	void Material::Cleanup()
	{
		for ( auto l_pass : m_passes )
		{
			l_pass->Cleanup();
		}
	}

	PassSPtr Material::CreatePass()
	{
		PassSPtr l_newPass;

		switch ( GetType() )
		{
		case MaterialType::eLegacy:
			l_newPass = std::make_shared< LegacyPass >( *this );
			break;

		case MaterialType::ePbrMetallicRoughness:
			l_newPass = std::make_shared< MetallicRoughnessPbrPass >( *this );
			break;

		case MaterialType::ePbrSpecularGlossiness:
			l_newPass = std::make_shared< SpecularGlossinessPbrPass >( *this );
			break;

		default:
			FAILURE( cuT( "Unsupported pass type" ) );
			break;
		}

		REQUIRE( l_newPass );
		m_passes.push_back( l_newPass );
		return l_newPass;
	}

	void Material::RemovePass( PassSPtr p_pass )
	{
		auto l_it = std::find( m_passes.begin(), m_passes.end(), p_pass );

		if ( l_it != m_passes.end() )
		{
			m_passes.erase( l_it );
		}
	}

	PassSPtr Material::GetPass( uint32_t p_index )const
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
		return m_passes.end() == std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassSPtr p_pass )
			{
				return !p_pass->HasAlphaBlending();
			} );
	}

	bool Material::HasEnvironmentMapping()const
	{
		return m_passes.end() != std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassSPtr p_pass )
			{
				return p_pass->HasEnvironmentMapping();
			} );
	}
}
