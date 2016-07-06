#include "HaarmPieterDuikerToneMapping.hpp"

#include <Logger.hpp>

#include <Engine.hpp>
#include <RenderTargetCache.hpp>

#include <Miscellaneous/Version.hpp>

using namespace HaarmPieterDuiker;

#ifndef _WIN32
#	define C3D_HaarmPieterDuikerToneMapping_API
#else
#	ifdef HaarmPieterDuikerToneMapping_EXPORTS
#		define C3D_HaarmPieterDuikerToneMapping_API __declspec( dllexport )
#	else
#		define C3D_HaarmPieterDuikerToneMapping_API __declspec( dllimport )
#	endif
#endif

C3D_HaarmPieterDuikerToneMapping_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version ();
}

C3D_HaarmPieterDuikerToneMapping_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_TONEMAPPING;
}

C3D_HaarmPieterDuikerToneMapping_API Castor::String GetName()
{
	return cuT( "Haarm Pieter Duiker Tone Mapping" );
}

C3D_HaarmPieterDuikerToneMapping_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetTargetCache().GetToneMappingFactory().Register( ToneMapping::Name, &ToneMapping::Create );
}

C3D_HaarmPieterDuikerToneMapping_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetTargetCache().GetToneMappingFactory().Unregister( ToneMapping::Name );
}
