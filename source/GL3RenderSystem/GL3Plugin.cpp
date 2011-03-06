#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3Plugin.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"

using namespace Castor3D;

extern "C" C3D_Gl3_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 5);
}

extern "C" C3D_Gl3_API void RegisterPlugin( Root & p_root)
{
	p_root.GetRendererServer().AddRenderer( RendererDriver::eOpenGl3, RendererDriverPtr( new Gl3RendererDriver()));
}

//******************************************************************************

const String & Gl3RendererDriver :: GetName() const
{
	static String l_name( "OpenGL 3.x graphics driver");
	return l_name;
}

void Gl3RendererDriver :: CreateRenderSystem( SceneManager * p_pSceneManager)
{
	new Gl3RenderSystem( p_pSceneManager);
}

//******************************************************************************
