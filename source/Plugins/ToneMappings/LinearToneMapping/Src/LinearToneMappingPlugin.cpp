#include "LinearToneMapping.hpp"

#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <TargetCache.hpp>

#include <Miscellaneous/Version.hpp>

using namespace Linear;

#ifndef _WIN32
#	define C3D_LinearToneMapping_API
#else
#	ifdef LinearToneMapping_EXPORTS
#		define C3D_LinearToneMapping_API __declspec( dllexport )
#	else
#		define C3D_LinearToneMapping_API __declspec( dllimport )
#	endif
#endif

C3D_LinearToneMapping_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_LinearToneMapping_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::ToneMapping;
}

C3D_LinearToneMapping_API Castor::String GetName()
{
	return cuT( "Linear Tone Mapping" );
}

C3D_LinearToneMapping_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetToneMappingFactory().Register( ToneMapping::Name, &ToneMapping::Create );
}

C3D_LinearToneMapping_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTargetCache().GetToneMappingFactory().Unregister( ToneMapping::Name );
}
