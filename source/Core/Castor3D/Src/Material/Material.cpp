#include "Material.hpp"

#include "LegacyPass.hpp"
#include "MetallicRoughnessPbrPass.hpp"
#include "SpecularGlossinessPbrPass.hpp"

#include "Scene/SceneFileParser.hpp"

using namespace castor;

namespace castor3d
{
	Material::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Material >{ p_tabs }
	{
	}

	bool Material::TextWriter::operator()( Material const & p_material, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing Material " ) + p_material.getName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "material \"" ) + p_material.getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< Material >::checkError( result, "Material name" );

		if ( result )
		{
			switch ( p_material.getType() )
			{
			case MaterialType::eLegacy:
				for ( auto pass : p_material )
				{
					result &= LegacyPass::TextWriter( m_tabs + cuT( "\t" ) )( *std::static_pointer_cast< LegacyPass >( pass ), p_file );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				for ( auto pass : p_material )
				{
					result &= MetallicRoughnessPbrPass::TextWriter( m_tabs + cuT( "\t" ) )( *std::static_pointer_cast< MetallicRoughnessPbrPass >( pass ), p_file );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				for ( auto pass : p_material )
				{
					result &= SpecularGlossinessPbrPass::TextWriter( m_tabs + cuT( "\t" ) )( *std::static_pointer_cast< SpecularGlossinessPbrPass >( pass ), p_file );
				}
				break;

			default:
				FAILURE( cuT( "Unsupported pass type" ) );
				break;
			}
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	const castor::String Material::DefaultMaterialName = cuT( "DefaultMaterial" );

	Material::Material( String const & p_name, Engine & engine, MaterialType p_type )
		: Resource< Material >( p_name )
		, OwnedBy< Engine >( engine )
		, m_type{ p_type }
	{
	}

	Material::~Material()
	{
	}

	void Material::initialise()
	{
		Logger::logDebug( cuT( "Initialising material [" ) + getName() + cuT( "]" ) );

		for ( auto pass : m_passes )
		{
			pass->initialise();
		}
	}

	void Material::cleanup()
	{
		for ( auto pass : m_passes )
		{
			pass->cleanup();
		}
	}

	PassSPtr Material::createPass()
	{
		PassSPtr newPass;

		switch ( getType() )
		{
		case MaterialType::eLegacy:
			newPass = std::make_shared< LegacyPass >( *this );
			break;

		case MaterialType::ePbrMetallicRoughness:
			newPass = std::make_shared< MetallicRoughnessPbrPass >( *this );
			break;

		case MaterialType::ePbrSpecularGlossiness:
			newPass = std::make_shared< SpecularGlossinessPbrPass >( *this );
			break;

		default:
			FAILURE( cuT( "Unsupported pass type" ) );
			break;
		}

		REQUIRE( newPass );
		m_passes.push_back( newPass );
		return newPass;
	}

	void Material::removePass( PassSPtr p_pass )
	{
		auto it = std::find( m_passes.begin(), m_passes.end(), p_pass );

		if ( it != m_passes.end() )
		{
			m_passes.erase( it );
		}
	}

	PassSPtr Material::getPass( uint32_t p_index )const
	{
		REQUIRE( p_index < m_passes.size() );
		return m_passes[p_index];
	}

	void Material::destroyPass( uint32_t p_index )
	{
		REQUIRE( p_index < m_passes.size() );
		m_passes.erase( m_passes.begin() + p_index );
	}

	bool Material::hasAlphaBlending()const
	{
		return m_passes.end() == std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassSPtr p_pass )
			{
				return !p_pass->hasAlphaBlending();
			} );
	}

	bool Material::hasEnvironmentMapping()const
	{
		return m_passes.end() != std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassSPtr p_pass )
			{
				return p_pass->hasEnvironmentMapping();
			} );
	}
}
