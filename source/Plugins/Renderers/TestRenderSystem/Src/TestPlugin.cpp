#include "Render/TestRenderSystem.hpp"

#include <Engine.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace TestRender;
using namespace Castor3D;
using namespace Castor;

C3D_Test_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Test_API PluginType GetType()
{
	return PluginType::eRenderer;
}

C3D_Test_API Castor::String GetRendererType()
{
	return TestRenderSystem::Name;
}

C3D_Test_API String GetName()
{
	return cuT( "Test Renderer" );
}

C3D_Test_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Register( TestRenderSystem::Name, TestRenderSystem::Create );
}

C3D_Test_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Unregister( TestRenderSystem::Name );
}
