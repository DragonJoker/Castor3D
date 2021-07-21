#include "DrawEdgesPostEffect/DrawEdgesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace draw_edges
{
	const castor::String DrawEdgesUbo::Name = cuT( "DrawEdgesUbo" );
	const castor::String DrawEdgesUbo::NormalDepthWidth = cuT( "c3d_normalDepthWidth" );
	const castor::String DrawEdgesUbo::ObjectWidth = cuT( "c3d_objectWidth" );

	DrawEdgesUbo::DrawEdgesUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPools->getBuffer< Configuration >( 0u ) }
	{
		auto & data = m_ubo.getData();
		data.normalDepthWidth = 1.0f;
		data.objectWidth = 1.0f;
	}

	DrawEdgesUbo::~DrawEdgesUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void DrawEdgesUbo::cpuUpdate( float normalDepthWidth
		, float objectWidth )
	{
		auto & data = m_ubo.getData();
		data.normalDepthWidth = normalDepthWidth;
		data.objectWidth = objectWidth;
	}

	//************************************************************************************************
}
