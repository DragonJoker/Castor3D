#include "BillboardUbo.hpp"

#include "Engine.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	BillboardUbo::BillboardUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferBillboards
			, *engine.GetRenderSystem()
			, BillboardUbo::BindingPoint }
		, m_dimensions{ *m_ubo.CreateUniform< UniformType::eVec2i >( ShaderProgram::Dimensions ) }
		, m_windowSize{ *m_ubo.CreateUniform< UniformType::eVec2i >( ShaderProgram::WindowSize ) }
	{
	}

	BillboardUbo::~BillboardUbo()
	{
	}

	void BillboardUbo::Update( Size const & p_dimensions )const
	{
		m_dimensions.SetValue( Point2i( p_dimensions[0], p_dimensions[1] ) );
		m_ubo.Update();
		m_ubo.BindTo( BillboardUbo::BindingPoint );
	}

	void BillboardUbo::SetWindowSize( Size const & p_window )const
	{
		m_windowSize.SetValue( Point2i( p_window[0], p_window[1] ) );
	}
}
