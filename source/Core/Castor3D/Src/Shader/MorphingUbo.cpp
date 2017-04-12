#include "MorphingUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	MorphingUbo::MorphingUbo( Engine & p_engine )
		: m_ubo{ ShaderProgram::BufferOverlay, *p_engine.GetRenderSystem() }
		, m_time{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::Time ) }
	{
	}

	MorphingUbo::~MorphingUbo()
	{
	}

	void MorphingUbo::Update( float p_time )const
	{
		m_time.SetValue( p_time );
		m_ubo.Update();
	}
}
