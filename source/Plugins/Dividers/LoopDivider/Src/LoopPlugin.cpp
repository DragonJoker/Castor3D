#include "LoopDivider.hpp"

#include <Engine.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
using namespace Loop;

C3D_Loop_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Loop_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::eDivider;
}

C3D_Loop_API Castor::String GetName()
{
	return Subdivider::Name;
}

C3D_Loop_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetSubdividerFactory().Register( Subdivider::Type, &Subdivider::Create );
}

C3D_Loop_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetSubdividerFactory().Unregister( Subdivider::Type );
}
