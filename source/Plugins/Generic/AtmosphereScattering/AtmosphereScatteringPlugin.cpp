#include "AtmosphereScattering/AtmosphereScatteringPrerequisites.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereLightingModel.hpp"
#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>

#ifndef CU_PlatformWindows
#	define C3D_AtmosphereScattering_API
#else
#	ifdef AtmosphereScattering_EXPORTS
#		define C3D_AtmosphereScattering_API __declspec( dllexport )
#	else
#		define C3D_AtmosphereScattering_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_AtmosphereScattering_API void getRequiredVersion( castor3d::Version * version );
	C3D_AtmosphereScattering_API void getType( castor3d::PluginType * type );
	C3D_AtmosphereScattering_API void getName( char const ** name );
	C3D_AtmosphereScattering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_AtmosphereScattering_API void OnUnload( castor3d::Engine * engine );

	C3D_AtmosphereScattering_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_AtmosphereScattering_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_AtmosphereScattering_API void getName( char const ** name )
	{
		*name = atmosphere_scattering::AtmosphereBackgroundModel::PluginName.c_str();
	}

	C3D_AtmosphereScattering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		auto backgroundModelId = engine->registerBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name
			, atmosphere_scattering::AtmosphereBackgroundModel::create );
		engine->registerPassModel( backgroundModelId
			, { castor::String{ atmosphere_scattering::AtmospherePhongLightingModel::getName() }
				, castor3d::PhongPass::create
				, &atmosphere_scattering::AtmospherePhongLightingModel::create
				, false } );
		engine->registerPassModel( backgroundModelId
			, { castor::String{ atmosphere_scattering::AtmospherePbrLightingModel::getName() }
				, castor3d::PbrPass::create
				, &atmosphere_scattering::AtmospherePbrLightingModel::create
				, true } );
		engine->registerParsers( atmosphere_scattering::AtmosphereBackgroundModel::PluginType
			, atmosphere_scattering::createParsers()
			, atmosphere_scattering::createSections()
			, nullptr );
	}

	C3D_AtmosphereScattering_API void OnUnload( castor3d::Engine * engine )
	{
		auto backgroundModelId = engine->unregisterBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name );
		engine->unregisterParsers( atmosphere_scattering::AtmosphereBackgroundModel::PluginType );
		engine->unregisterPassModel( backgroundModelId
			, engine->getPassFactory().getNameId( castor::String{ atmosphere_scattering::AtmospherePhongLightingModel::getName() } ) );
		engine->unregisterPassModel( backgroundModelId
			, engine->getPassFactory().getNameId( castor::String{ atmosphere_scattering::AtmospherePbrLightingModel::getName() } ) );
	}
}
