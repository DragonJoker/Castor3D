#include "FxaaUbo.hpp"

#include <Engine.hpp>

using namespace castor;
using namespace castor3d;

namespace fxaa
{
	const String FxaaUbo::Name = cuT( "FxaaUbo" );
	const String FxaaUbo::SubpixShift = cuT( "c3d_subpixShift" );
	const String FxaaUbo::SpanMax = cuT( "c3d_spanMax" );
	const String FxaaUbo::ReduceMul = cuT( "c3d_reduceMul" );
	const String FxaaUbo::RenderSize = cuT( "c3d_renderSize" );

	FxaaUbo::FxaaUbo( Engine & engine )
		: m_ubo{ renderer::makeUniformBuffer< Configuration >( *engine.getRenderSystem()->getCurrentDevice()
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent ) }
	{
	}

	void FxaaUbo::update( Size const & size
		, float shift
		, float span
		, float reduce )
	{
		auto & data = m_ubo->getData();
		data.subpixShift = shift;
		data.spanMax = span;
		data.reduceMul = reduce;
		data.renderSize = Point2f{ size.getWidth(), size.getHeight() };
		m_ubo->upload();
	}

	//************************************************************************************************
}
