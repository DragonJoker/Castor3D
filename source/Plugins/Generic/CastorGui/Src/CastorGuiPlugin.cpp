#include <Engine.hpp>
#include <RenderSystem.hpp>

#include <Logger.hpp>

#include "CastorGuiPrerequisites.hpp"

using namespace Castor3D;
using namespace Castor;

C3D_Ssao_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Ssao_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_POSTFX;
}

C3D_Ssao_API String GetName()
{
	return cuT( "Castor GUI" );
}

C3D_Cgui_API void AddOptionalParsers( SceneFileParser * p_parser )
{
}
