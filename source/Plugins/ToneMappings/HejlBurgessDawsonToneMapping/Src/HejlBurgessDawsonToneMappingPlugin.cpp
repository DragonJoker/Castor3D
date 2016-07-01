#include "HejlBurgessDawsonToneMapping.hpp"

#include <Logger.hpp>

#include <Engine.hpp>
#include <TargetManager.hpp>

#include <Miscellaneous/Version.hpp>

using namespace HejlBurgessDawson;

#ifndef _WIN32
#	define C3D_HejlBurgessDawsonToneMapping_API
#else
#	ifdef HejlBurgessDawsonToneMapping_EXPORTS
#		define C3D_HejlBurgessDawsonToneMapping_API __declspec( dllexport )
#	else
#		define C3D_HejlBurgessDawsonToneMapping_API __declspec( dllimport )
#	endif
#endif

C3D_HejlBurgessDawsonToneMapping_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version ();
}

C3D_HejlBurgessDawsonToneMapping_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_TONEMAPPING;
}

C3D_HejlBurgessDawsonToneMapping_API Castor::String GetName()
{
	return cuT( "Hejl Burgess Dawson Tone Mapping" );
}

C3D_HejlBurgessDawsonToneMapping_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetTargetManager().GetToneMappingFactory().Register( ToneMapping::Name, &ToneMapping::Create );
}

C3D_HejlBurgessDawsonToneMapping_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetTargetManager().GetToneMappingFactory().Unregister( ToneMapping::Name );
}
