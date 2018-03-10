/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/Renderer.hpp"

namespace renderer
{
	Renderer::Renderer( ClipDirection clipDirection
		, std::string const & name
		, Configuration const & configuration )
		: m_configuration{ configuration }
		, m_clipDirection{ clipDirection }
		, m_name{ name }
	{
	}

	PhysicalDevice & Renderer::getPhysicalDevice( uint32_t gpuIndex )const
	{
		if ( gpuIndex >= m_gpus.size() )
		{
			throw std::logic_error( "Invalid GPU index" );
		}

		return *m_gpus[gpuIndex];
	}
}
