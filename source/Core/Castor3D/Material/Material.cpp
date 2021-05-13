#include "Castor3D/Material/Material.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp"
#include "Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp"

using namespace castor;

namespace castor3d
{
	const castor::String Material::DefaultMaterialName = cuT( "DefaultMaterial" );

	Material::Material( String const & name
		, Engine & engine
		, MaterialType type )
		: Resource< Material >( name )
		, OwnedBy< Engine >( engine )
		, m_type{ type }
	{
	}

	Material::~Material()
	{
	}

	void Material::initialise( RenderDevice const & device )
	{
		log::debug << cuT( "Initialising material [" ) << getName() << cuT( "]" ) << std::endl;

		for ( auto pass : m_passes )
		{
			pass->initialise( device );
		}

		getEngine()->getMaterialCache().registerMaterial( *this );
	}

	void Material::cleanup()
	{
		getEngine()->getMaterialCache().unregisterMaterial( *this );

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
		case MaterialType::ePhong:
			newPass = std::make_shared< PhongPass >( *this );
			break;

		case MaterialType::eMetallicRoughness:
			newPass = std::make_shared< MetallicRoughnessPbrPass >( *this );
			break;

		case MaterialType::eSpecularGlossiness:
			newPass = std::make_shared< SpecularGlossinessPbrPass >( *this );
			break;

		default:
			CU_Failure( cuT( "Unsupported pass type" ) );
			break;
		}

		CU_Require( newPass );
		m_passListeners.emplace( newPass, newPass->onChanged.connect( [this]( Pass const & pass )
			{
				onPassChanged( pass );
			} ) );
		m_passes.push_back( newPass );
		onChanged( *this );
		return newPass;
	}

	void Material::removePass( PassSPtr pass )
	{
		auto it = std::find( m_passes.begin(), m_passes.end(), pass );

		if ( it != m_passes.end() )
		{
			m_passListeners.erase( *it );
			m_passes.erase( it );
			onChanged( *this );
		}
	}

	PassSPtr Material::getPass( uint32_t index )const
	{
		CU_Require( index < m_passes.size() );
		return m_passes[index];
	}

	void Material::destroyPass( uint32_t index )
	{
		CU_Require( index < m_passes.size() );
		m_passListeners.erase( *( m_passes.begin() + index ) );
		m_passes.erase( m_passes.begin() + index );
		onChanged( *this );
	}

	bool Material::hasAlphaBlending()const
	{
		return m_passes.end() == std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassSPtr pass )
			{
				return !pass->hasAlphaBlending();
			} );
	}

	bool Material::hasEnvironmentMapping()const
	{
		return m_passes.end() != std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassSPtr pass )
			{
				return pass->hasEnvironmentMapping();
			} );
	}

	bool Material::hasSubsurfaceScattering()const
	{
		return m_passes.end() != std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassSPtr pass )
			{
				return pass->hasSubsurfaceScattering();
			} );
	}

	bool Material::isTextured( TextureFlags mask )const
	{
		return m_passes.end() != std::find_if( m_passes.begin()
			, m_passes.end()
			, [mask]( PassSPtr pass )
			{
				return pass->getTextureUnitsCount( mask ) > 0;
			} );
	}

	void Material::onPassChanged( Pass const & pass )
	{
		onChanged( *this );
	}
}
