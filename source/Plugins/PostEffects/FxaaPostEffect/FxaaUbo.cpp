#include "FxaaPostEffect/FxaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace fxaa
{
	//*********************************************************************************************

	c3d::MbString const FxaaUbo::Buffer = "Fxaa";
	c3d::MbString const FxaaUbo::Data = "c3d_fxaaData";

	FxaaUbo::FxaaUbo( c3d::RenderDevice const & device
		, c3d::Size const & size )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( c3d::MemoryPropertyFlags::eNone ) }
	{
		auto & data = m_ubo.getData();
		data.pixelSize = c3d::Point2f{ 1.0f / float( size.getWidth() )
			, 1.0f / float( size.getHeight() ) };
	}

	FxaaUbo::~FxaaUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
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
