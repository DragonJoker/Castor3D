#include "FxaaUbo.hpp"

#include <Engine.hpp>

namespace fxaa
{
	const castor::String FxaaUbo::Name = cuT( "FxaaUbo" );
	const castor::String FxaaUbo::SubpixShift = cuT( "c3d_subpixShift" );
	const castor::String FxaaUbo::SpanMax = cuT( "c3d_spanMax" );
	const castor::String FxaaUbo::ReduceMul = cuT( "c3d_reduceMul" );
	const castor::String FxaaUbo::PixelSize = cuT( "c3d_pixelSize" );

	FxaaUbo::FxaaUbo( castor3d::Engine & engine
		, castor::Size const & size )
		: m_ubo{ renderer::makeUniformBuffer< Configuration >( *engine.getRenderSystem()->getCurrentDevice()
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent ) }
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
