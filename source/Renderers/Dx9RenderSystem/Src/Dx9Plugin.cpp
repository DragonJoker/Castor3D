#include "Dx9RenderSystem.hpp"

#include <Version.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

C3D_Dx9_API void GetRequiredVersion( Version & p_version )
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
	return cuT( "Direct3D9 Renderer" );
}

C3D_Dx9_API RenderSystem * CreateRenderSystem( Engine * p_pEngine )
{
	return new DxRenderSystem( p_pEngine );
}

C3D_Dx9_API void DestroyRenderSystem( RenderSystem * p_pRenderSystem )
{
	delete p_pRenderSystem;
}

C3D_Dx9_API void OnLoad( Castor3D::Engine * p_engine )
{
	Castor::Logger::Initialise( p_engine->GetLoggerInstance() );
}

C3D_Dx9_API void OnUnload( Castor3D::Engine * p_engine )
{
	Castor::Logger::Cleanup();
}

