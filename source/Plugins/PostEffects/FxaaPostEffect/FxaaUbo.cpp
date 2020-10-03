#include "FxaaPostEffect/FxaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace fxaa
{
	const castor::String FxaaUbo::Name = cuT( "FxaaUbo" );
	const castor::String FxaaUbo::SubpixShift = cuT( "c3d_subpixShift" );
	const castor::String FxaaUbo::SpanMax = cuT( "c3d_spanMax" );
	const castor::String FxaaUbo::ReduceMul = cuT( "c3d_reduceMul" );
	const castor::String FxaaUbo::PixelSize = cuT( "c3d_pixelSize" );

	FxaaUbo::FxaaUbo( castor3d::RenderDevice const & device
		, castor::Size const & size )
		: m_device{ device }
		, m_ubo{ device.uboPools->getBuffer< Configuration >( 0u ) }
	{
		auto & data = m_ubo.getData();
		data.pixelSize = castor::Point2f{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
	}

	FxaaUbo::~FxaaUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void FxaaUbo::cpuUpdate( float shift
		, float span
		, float reduce )
	{
		auto & data = m_ubo.getData();
		data.subpixShift = shift;
		data.spanMax = span;
		data.reduceMul = reduce;
	}

	//************************************************************************************************
}
