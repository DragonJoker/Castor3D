#include "HdrConfigUbo.hpp"

#include "Engine.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	HdrConfigUbo::HdrConfigUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferHdrConfig, *engine.GetRenderSystem() }
		, m_exposure{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::Exposure ) }
		, m_gamma{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::Gamma ) }
	{
	}

	HdrConfigUbo::~HdrConfigUbo()
	{
	}

	void HdrConfigUbo::Update( HdrConfig const & p_config )const
	{
		m_exposure.SetValue( p_config.GetExposure() );
		m_gamma.SetValue( p_config.GetGamma() );
		m_ubo.Update();
		m_ubo.BindTo( HdrConfigUbo::BindingPoint );
	}
}
