#include "FxaaUbo.hpp"

#include <Engine.hpp>

using namespace Castor;
using namespace Castor3D;

namespace fxaa
{
	const String FxaaUbo::Name = cuT( "FxaaUbo" );
	const String FxaaUbo::SubpixShift = cuT( "c3d_subpixShift" );
	const String FxaaUbo::SpanMax = cuT( "c3d_spanMax" );
	const String FxaaUbo::ReduceMul = cuT( "c3d_reduceMul" );
	const String FxaaUbo::RenderSize = cuT( "c3d_renderSize" );

	FxaaUbo::FxaaUbo( Engine & engine )
		: m_ubo{ FxaaUbo::Name
			, *engine.GetRenderSystem()
			, FxaaUbo::BindingPoint }
		, m_subpixShift{ m_ubo.CreateUniform< UniformType::eFloat >( FxaaUbo::SubpixShift ) }
		, m_spanMax{ m_ubo.CreateUniform< UniformType::eFloat >( FxaaUbo::SpanMax ) }
		, m_reduceMul{ m_ubo.CreateUniform< UniformType::eFloat >( FxaaUbo::ReduceMul ) }
		, m_renderSize{ m_ubo.CreateUniform< UniformType::eVec2f >( FxaaUbo::RenderSize ) }
	{
	}

	FxaaUbo::~FxaaUbo()
	{
		m_ubo.Cleanup();
	}

	void FxaaUbo::Update( Size const & p_size
		, float p_shift
		, float p_span
		, float p_reduce )
	{
		m_subpixShift->SetValue( p_shift );
		m_spanMax->SetValue( p_span );
		m_reduceMul->SetValue( p_reduce );
		m_renderSize->SetValue( Point2f( p_size.width(), p_size.height() ) );
		m_ubo.Update();
		m_ubo.BindTo( FxaaUbo::BindingPoint );
	}

	//************************************************************************************************
}
