#include "Castor3D/Material/Material.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	const castor::String Material::DefaultMaterialName = cuT( "C3D_DefaultMaterial" );

	Material::Material( castor::String const & name
		, Engine & engine
		, LightingModelID lightingModelId )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ engine }
		, m_lightingModelId{ lightingModelId }
	{
	}

	void Material::initialise()
	{
		log::debug << cuT( "Initialising material [" ) << getName() << cuT( "]" ) << std::endl;

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

	PassSPtr Material::createPass( LightingModelID lightingModelId )
	{
		if ( m_passes.size() == MaxPassLayers )
		{
			log::error << cuT( "Can't create a new pass, max pass count reached" ) << std::endl;
			CU_Failure( "Can't create a new pass, max pass count reached" );
			return nullptr;
		}

		auto result = getEngine()->getPassFactory().create( lightingModelId
			, *this );
		CU_Require( result );
		m_passListeners.emplace( result
			, result->onChanged.connect( [this]( Pass const & p )
			{
				onPassChanged( p );
			} ) );
		m_passes.push_back( result );
		onChanged( *this );
		return result;
	}

	PassSPtr Material::createPass()
	{
		return createPass( m_lightingModelId );
	}

	void Material::addPass( Pass const & pass )
	{
		if ( m_passes.size() == MaxPassLayers )
		{
			log::error << cuT( "Can't create a new pass, max pass count reached" ) << std::endl;
			CU_Failure( "Can't create a new pass, max pass count reached" );
			return;
		}

		auto newPass = pass.clone( *this );
		CU_Require( newPass );
		m_passListeners.emplace( newPass
			, newPass->onChanged.connect( [this]( Pass const & p )
			{
				onPassChanged( p );
			} ) );
		m_passes.push_back( newPass );
		onChanged( *this );
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

	void Material::onPassChanged( Pass const & pass )
	{
		onChanged( *this );
	}
}
