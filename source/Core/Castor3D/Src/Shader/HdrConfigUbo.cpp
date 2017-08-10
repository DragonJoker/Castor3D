#include "HdrConfigUbo.hpp"

#include "Engine.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	HdrConfigUbo::HdrConfigUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferHdrConfig
			, *engine.getRenderSystem()
			, HdrConfigUbo::BindingPoint }
		, m_exposure{ *m_ubo.createUniform< UniformType::eFloat >( ShaderProgram::Exposure ) }
		, m_gamma{ *m_ubo.createUniform< UniformType::eFloat >( ShaderProgram::Gamma ) }
	{
	}

	HdrConfigUbo::~HdrConfigUbo()
	{
	}

	void HdrConfigUbo::update( HdrConfig const & p_config )const
	{
		m_exposure.setValue( p_config.getExposure() );
		m_gamma.setValue( p_config.getGamma() );
		m_ubo.update();
		m_ubo.bindTo( HdrConfigUbo::BindingPoint );
	}
}
