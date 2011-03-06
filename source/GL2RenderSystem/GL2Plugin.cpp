#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2Plugin.h"
#include "Gl2RenderSystem/Gl2RenderSystem.h"

using namespace Castor3D;

//******************************************************************************

extern "C" C3D_Gl2_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 5);
}

extern "C" C3D_Gl2_API void RegisterPlugin( Root & p_root)
{
	p_root.GetRendererServer().AddRenderer( RendererDriver::eOpenGl2, RendererDriverPtr( new Gl2RendererDriver()));
}

//******************************************************************************

const String & Gl2RendererDriver :: GetName() const
{
	static String l_name( "OpenGL 2.x graphics driver");
	return l_name;
}

void Gl2RendererDriver :: CreateRenderSystem( SceneManager * p_pSceneManager)
{
	new Gl2RenderSystem( p_pSceneManager);
}

//******************************************************************************
