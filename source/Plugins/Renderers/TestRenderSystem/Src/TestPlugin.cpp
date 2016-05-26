#include "Render/TestRenderSystem.hpp"

#include <Engine.hpp>

using namespace TestRender;
using namespace Castor3D;
using namespace Castor;

C3D_Test_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Test_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_RENDERER;
}

C3D_Test_API Castor::String GetRendererType()
{
	return TestRenderSystem::Name;
}

C3D_Test_API String GetName()
{
	return cuT( "Test Renderer" );
}

C3D_Test_API RenderSystem * CreateRenderSystem( Engine * p_engine )
{
	return new TestRenderSystem( *p_engine );
}

C3D_Test_API void DestroyRenderSystem( RenderSystem * p_renderSystem )
{
	delete p_renderSystem;
}

C3D_Test_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Test_API void OnUnload( Castor3D::Engine * p_engine )
{
}
