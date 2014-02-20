#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxPlugin.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

//******************************************************************************

C3D_Dx9_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Dx9_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_RENDERER;
}

C3D_Dx9_API eRENDERER_TYPE GetRendererType()
{
	return eRENDERER_TYPE_DIRECT3D9;
}

C3D_Dx9_API String GetName()
{
	return cuT( "Direct3D9 Renderer");
}

C3D_Dx9_API RenderSystem * CreateRenderSystem( Engine * p_pEngine, Logger * p_pLogger )
{
	return new DxRenderSystem( p_pEngine, p_pLogger );
}

C3D_Dx9_API void DestroyRenderSystem( RenderSystem * p_pRenderSystem )
{
	delete p_pRenderSystem;
}

//******************************************************************************
