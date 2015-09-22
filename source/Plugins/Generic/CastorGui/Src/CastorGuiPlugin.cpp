#include <Engine.hpp>
#include <RenderSystem.hpp>

#include <Logger.hpp>

#include "CastorGuiPrerequisites.hpp"

using namespace Castor3D;
using namespace Castor;

C3D_CGui_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_CGui_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_GENERIC;
}

C3D_CGui_API String GetName()
{
	return cuT( "Castor GUI" );
}

C3D_CGui_API void AddOptionalParsers( SceneFileParser * p_parser )
{
}
