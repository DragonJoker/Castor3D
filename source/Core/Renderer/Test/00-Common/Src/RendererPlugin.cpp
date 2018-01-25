#include "RendererPlugin.hpp"

#include <Core/Renderer.hpp>

#include <functional>

namespace common
{
	RendererPlugin::RendererPlugin( renderer::DynamicLibrary && library
		, RendererFactory & factory )
		: m_library{ std::move( library ) }
		, m_creator{ nullptr }
	{
		if ( !m_library.getFunction( "createRenderer", m_creator ) )
		{
			throw std::runtime_error{ "Not a renderer plugin" };
		}

		std::string name;

		if ( m_library.getPath().find( "GlR" ) != std::string::npos )
		{
			name = "gl";
		}
		else if ( m_library.getPath().find( "VkR" ) != std::string::npos )
		{
			name = "vk";
		}
		else
		{
			throw std::runtime_error{ "Not a supported renderer plugin" };
		}

		auto creator = m_creator;
		factory.registerType( name, [creator]()
			{
				return renderer::RendererPtr{ creator() };
			} );
	}

	renderer::RendererPtr RendererPlugin::create()
	{
		return renderer::RendererPtr{ m_creator() };
	}
}
