#include "HdrConfigUbo.hpp"

#include "Engine.hpp"
#include "HDR/HdrConfig.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	String const HdrConfigUbo::BufferHdrConfig = cuT( "HdrConfig" );
	String const HdrConfigUbo::Exposure = cuT( "c3d_exposure" );
	String const HdrConfigUbo::Gamma = cuT( "c3d_gamma" );

	HdrConfigUbo::HdrConfigUbo( Engine & engine )
		: m_ubo{ HdrConfigUbo::BufferHdrConfig
			, *engine.getRenderSystem()
			, HdrConfigUbo::BindingPoint }
		, m_exposure{ *m_ubo.createUniform< UniformType::eFloat >( HdrConfigUbo::Exposure ) }
		, m_gamma{ *m_ubo.createUniform< UniformType::eFloat >( HdrConfigUbo::Gamma ) }
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
