#include "FxaaPostEffect/FxaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBuffer.hpp>

namespace fxaa
{
	const castor::String FxaaUbo::Name = cuT( "FxaaUbo" );
	const castor::String FxaaUbo::SubpixShift = cuT( "c3d_subpixShift" );
	const castor::String FxaaUbo::SpanMax = cuT( "c3d_spanMax" );
	const castor::String FxaaUbo::ReduceMul = cuT( "c3d_reduceMul" );
	const castor::String FxaaUbo::PixelSize = cuT( "c3d_pixelSize" );

	FxaaUbo::FxaaUbo( castor3d::Engine & engine
		, castor::Size const & size )
		: m_ubo{ castor3d::makeUniformBuffer< Configuration >( *engine.getRenderSystem()
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, "FxaaCfg" ) }
	{
		auto & data = m_ubo->getData();
		data.pixelSize = castor::Point2f{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
	}

	void FxaaUbo::update( float shift
		, float span
		, float reduce )
	{
		auto & data = m_ubo->getData();
		data.subpixShift = shift;
		data.spanMax = span;
		data.reduceMul = reduce;
		m_ubo->upload();
	}

	//************************************************************************************************
}
