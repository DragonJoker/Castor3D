#include "BillboardUbo.hpp"

#include "Engine.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	BillboardUbo::BillboardUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferBillboards
			, *engine.getRenderSystem()
			, BillboardUbo::BindingPoint }
		, m_dimensions{ *m_ubo.createUniform< UniformType::eVec2i >( ShaderProgram::Dimensions ) }
		, m_windowSize{ *m_ubo.createUniform< UniformType::eVec2i >( ShaderProgram::WindowSize ) }
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
