#include "BillboardUbo.hpp"

#include "Engine.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	String const BillboardUbo::BufferBillboard = cuT( "Billboard" );
	String const BillboardUbo::Dimensions = cuT( "c3d_dimensions" );
	String const BillboardUbo::WindowSize = cuT( "c3d_windowSize" );

	BillboardUbo::BillboardUbo( Engine & engine )
		: m_ubo{ BillboardUbo::BufferBillboard
			, *engine.getRenderSystem()
			, BillboardUbo::BindingPoint }
		, m_dimensions{ *m_ubo.createUniform< UniformType::eVec2i >( BillboardUbo::Dimensions ) }
		, m_windowSize{ *m_ubo.createUniform< UniformType::eVec2i >( BillboardUbo::WindowSize ) }
	{
	}

	BillboardUbo::~BillboardUbo()
	{
	}

	void BillboardUbo::update( Size const & p_dimensions )const
	{
		m_dimensions.setValue( Point2i( p_dimensions[0], p_dimensions[1] ) );
		m_ubo.update();
		m_ubo.bindTo( BillboardUbo::BindingPoint );
	}

	void BillboardUbo::setWindowSize( Size const & p_window )const
	{
		m_windowSize.setValue( Point2i( p_window[0], p_window[1] ) );
	}
}
