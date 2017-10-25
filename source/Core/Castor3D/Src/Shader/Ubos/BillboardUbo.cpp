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

	void BillboardUbo::update( Size const & dimensions )const
	{
		m_dimensions.setValue( Point2i( dimensions[0], dimensions[1] ) );
		m_ubo.update();
		m_ubo.bindTo( BillboardUbo::BindingPoint );
	}

	void BillboardUbo::setWindowSize( Size const & window )const
	{
		m_windowSize.setValue( Point2i( window[0], window[1] ) );
	}
}
