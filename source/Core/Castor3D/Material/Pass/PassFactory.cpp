#include "Castor3D/Material/Pass/PassFactory.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"

CU_ImplementCUSmartPtr( castor3d, PassFactory )

namespace castor3d
{
	PassFactory::PassFactory( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, PassFactoryBase{}
	{
		registerType( PhongPass::Type
			, { PhongPass::LightingModel
				, PhongPass::create
				, &shader::PhongLightingModel::create
				, false } );
		registerType( PbrPass::Type
			, { PbrPass::LightingModel
				, PbrPass::create
				, &shader::PbrLightingModel::create
				, true } );
	}

	PassFactory::~PassFactory()
	{
	}

	void PassFactory::registerType( castor::String const & passType
		, PassRegisterInfo info )
	{
		auto id = uint16_t( m_passTypeNames.size() + 1u );
		PassFactoryBase::registerType( id, info.passCreator );
		m_passTypeNames.emplace_back( passType, id );
		m_lightingModels.emplace( id, info.lightingModel );
		getEngine()->registerLightingModel( info.lightingModel, info.lightingModelCreator );
	}

	void PassFactory::unregisterType( castor::String const & passType )
	{
		auto it = std::find_if( m_passTypeNames.begin()
			, m_passTypeNames.end()
			, [&passType]( StringIdPair const & lookup )
			{
				return lookup.first == passType;
			} );
		CU_Require( it != m_passTypeNames.end() );
		auto id = it->second;
		auto lightingIt = m_lightingModels.find( id );
		CU_Require( lightingIt != m_lightingModels.end() );
		getEngine()->unregisterLightingModel( lightingIt->second );
		m_lightingModels.erase( lightingIt );
		m_passTypeNames.erase( it );
		PassFactoryBase::unregisterType( id );
	}

	PassSPtr PassFactory::create( castor::String const & passType
		, Material & parent )const
	{
		return create( getNameId( passType ), parent );
	}

	PassTypeID PassFactory::getNameId( castor::String const & passType )const
	{
		auto it = std::find_if( m_passTypeNames.begin()
			, m_passTypeNames.end()
			, [passType]( StringIdPair const & lookup )
			{
				return lookup.first == passType;
			} );

		if ( it == m_passTypeNames.end() )
		{
			CU_Exception( "Unknown pass type ID." );
		}

		return it->second;
	}

	castor::String PassFactory::getIdName( PassTypeID passTypeId )const
	{
		auto it = std::find_if( m_passTypeNames.begin()
			, m_passTypeNames.end()
			, [passTypeId]( StringIdPair const & lookup )
			{
				return lookup.second == passTypeId;
			} );

		if ( it == m_passTypeNames.end() )
		{
			CU_Exception( "Unknown pass type ID." );
		}

		return it->first;
	}

	castor::String PassFactory::getLightingModelName( PassTypeID passTypeId )const
	{
		auto it = m_lightingModels.find( passTypeId );

		if ( it == m_lightingModels.end() )
		{
			CU_Exception( "Unknown pass type ID." );
		}

		return it->second;
	}
}
