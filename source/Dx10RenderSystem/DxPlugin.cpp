#include "Dx10RenderSystem/PrecompiledHeader.hpp"

#include "Dx10RenderSystem/DxPlugin.hpp"
#include "Dx10RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx10Render;

//******************************************************************************

C3D_Dx10_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Dx10_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_RENDERER;
}

C3D_Dx10_API eRENDERER_TYPE GetRendererType()
{
	return eRENDERER_TYPE_DIRECT3D10;
}

C3D_Dx10_API String GetName()
{
	return cuT( "Direct3D10 Renderer" );
}

C3D_Dx10_API RenderSystem * CreateRenderSystem( Engine * p_pEngine, Logger * p_pLogger )
{
	return new DxRenderSystem( p_pEngine, p_pLogger );
}

C3D_Dx10_API void DestroyRenderSystem( RenderSystem * p_pRenderSystem )
{
	delete p_pRenderSystem;
}

//******************************************************************************
