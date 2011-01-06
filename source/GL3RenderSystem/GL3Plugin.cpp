#include "GL3RenderSystem/PrecompiledHeader.h"

#include "GL3RenderSystem/GL3Plugin.h"
#include "GL3RenderSystem/GL3RenderSystem.h"

using namespace Castor3D;

extern "C" C3D_GL3_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 5);
}

extern "C" C3D_GL3_API void RegisterPlugin( Root & p_root)
{
	p_root.GetRendererServer().AddRenderer( RendererDriver::eOpenGL3, RendererDriverPtr( new GL3RendererDriver()));
}

//******************************************************************************

const String & GL3RendererDriver :: GetName() const
{
	static String l_name( "OpenGL 3.x graphics driver");
	return l_name;
}

void GL3RendererDriver :: CreateRenderSystem( SceneManager * p_pSceneManager)
{
	new GL3RenderSystem( p_pSceneManager);
}

//******************************************************************************
