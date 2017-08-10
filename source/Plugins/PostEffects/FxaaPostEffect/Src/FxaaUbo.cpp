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
		: m_ubo{ FxaaUbo::Name
			, *engine.getRenderSystem()
			, FxaaUbo::BindingPoint }
		, m_subpixShift{ m_ubo.createUniform< UniformType::eFloat >( FxaaUbo::SubpixShift ) }
		, m_spanMax{ m_ubo.createUniform< UniformType::eFloat >( FxaaUbo::SpanMax ) }
		, m_reduceMul{ m_ubo.createUniform< UniformType::eFloat >( FxaaUbo::ReduceMul ) }
		, m_renderSize{ m_ubo.createUniform< UniformType::eVec2f >( FxaaUbo::RenderSize ) }
	{
	}

	FxaaUbo::~FxaaUbo()
	{
		m_ubo.cleanup();
	}

	void FxaaUbo::update( Size const & p_size
		, float p_shift
		, float p_span
		, float p_reduce )
	{
		m_subpixShift->setValue( p_shift );
		m_spanMax->setValue( p_span );
		m_reduceMul->setValue( p_reduce );
		m_renderSize->setValue( Point2f( p_size.getWidth(), p_size.getHeight() ) );
		m_ubo.update();
		m_ubo.bindTo( FxaaUbo::BindingPoint );
	}

	//************************************************************************************************
}
