#pragma once

#include <Prerequisites.hpp>

namespace vkapp
{
	static wxString const AppName{ wxT( "01-DeviceCreation" ) };

	class Application;
	class MainFrame;
	class RenderingResources;
	class RenderPanel;

	using RenderingResourcesPtr = std::unique_ptr< RenderingResources >;
}
