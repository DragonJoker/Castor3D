#include "DrawEdgesPostEffect/DrawEdgesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace draw_edges
{
	//*********************************************************************************************

	c3d::MbString const DrawEdgesUbo::Buffer = "DrawEdges";
	c3d::MbString const DrawEdgesUbo::Data = "c3d_drawEdgesData";

	DrawEdgesUbo::DrawEdgesUbo( c3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( c3d::MemoryPropertyFlags::eNone ) }
	{
		auto & data = m_ubo.getData();
		data.normalDepthWidth = 1;
		data.objectWidth = 1;
	}

	DrawEdgesUbo::~DrawEdgesUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void DrawEdgesUbo::cpuUpdate( int normalDepthWidth
		, int objectWidth )
	{
		auto & data = m_ubo.getData();
		data.normalDepthWidth = normalDepthWidth;
		data.objectWidth = objectWidth;
	}

	//************************************************************************************************
}
