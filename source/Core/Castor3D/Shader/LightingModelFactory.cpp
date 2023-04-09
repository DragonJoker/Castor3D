#include "Castor3D/Shader/LightingModelFactory.hpp"

#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"

#include <CastorUtils/Exception/Exception.hpp>

CU_ImplementSmartPtr( castor3d, LightingModelFactory )

namespace castor3d
{
	namespace lgtmdlfct
	{
		static const castor::String TOO_MANY_OBJECTS = cuT( "Too many lighting models declared" );
	}

	LightingModelID LightingModelFactory::registerType( Key const & key
		, BackgroundModelID backgroundModelId
		, Creator create )
	{
		LightingModelID lightingModelId{};
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&key]( Entry const & lookup )
			{
				return key == lookup.name;
			} );

		if ( it == m_registered.end() )
		{
			if ( m_currentId > MaxLightingModels )
			{
				CU_Exception( lgtmdlfct::TOO_MANY_OBJECTS );
			}

			lightingModelId = ++m_currentId;
		}
		else
		{
			lightingModelId = it->lightingModelId;
		}

		auto & entry = m_registered.emplace_back();
		entry.name = key;
		entry.create = create;
		entry.lightingModelId = lightingModelId;
		entry.backgroundModelId = backgroundModelId;
		return lightingModelId;
	}

	void LightingModelFactory::unregisterType( Key const & key
		, BackgroundModelID backgroundModelId )
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&key, backgroundModelId]( Entry const & lookup )
			{
				return key == lookup.name
				&& backgroundModelId == lookup.backgroundModelId;
			} );

		if ( it != m_registered.end() )
		{
			m_registered.erase( it );
		}
	}

	void LightingModelFactory::unregisterType( LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId )
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [lightingModelId, backgroundModelId]( Entry const & lookup )
			{
				return lightingModelId == lookup.lightingModelId
				&& backgroundModelId == lookup.backgroundModelId;
			} );

		if ( it != m_registered.end() )
		{
			m_registered.erase( it );
		}
	}

	LightingModelFactory::Id LightingModelFactory::getNameId( Key const & key )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&key]( Entry const & lookup )
			{
				return key == lookup.name;
			} );

		return it == m_registered.end()
			? 0u
			: it->lightingModelId;
	}

	LightingModelFactory::Key LightingModelFactory::getIdName( Id const & id )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&id]( Entry const & lookup )
			{
				return id == lookup.lightingModelId;
			} );

		return it == m_registered.end()
			? Key{}
		: it->name;
	}

	std::vector< LightingModelID > LightingModelFactory::getLightingModelsID()const
	{
		std::unordered_set< LightingModelID > result;

		for ( auto & entry : m_registered )
		{
			result.emplace( entry.lightingModelId );
		}

		return { result.begin(), result.end() };
	}

	std::vector< LightingModelID > LightingModelFactory::getLightingModelsID( BackgroundModelID backgroundModelId )const
	{
		std::vector< LightingModelID > result;

		for ( auto & entry : m_registered )
		{
			if ( entry.backgroundModelId == backgroundModelId )
			{
				result.push_back( entry.lightingModelId );
			}
		}

		return result;
	}

	castor::String LightingModelFactory::normaliseName( castor::String name )
	{
		if ( name == "blinn_phong" || name == "phong" )
		{
			name = castor3d::PhongPass::LightingModel;
		}
		else if ( name == "pbr" )
		{
			name = castor3d::PbrPass::LightingModel;
		}

		return name;
	}
}
