#include "ModelUbo.hpp"

#include "Engine.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	ModelUbo::ModelUbo( Engine & p_engine )
		: m_ubo{ ShaderProgram::BufferModel, *p_engine.GetRenderSystem() }
		, m_shadowReceiver{ *m_ubo.CreateUniform< UniformType::eInt >( ShaderProgram::ShadowReceiver ) }
		, m_materialIndex{ *m_ubo.CreateUniform< UniformType::eInt >( ShaderProgram::MaterialIndex ) }
		, m_environmentIndex{ *m_ubo.CreateUniform< UniformType::eInt >( ShaderProgram::EnvironmentIndex ) }
	{
	}

	ModelUbo::~ModelUbo()
	{
	}

	void ModelUbo::SetEnvMapIndex( uint32_t p_value )
	{
		m_environmentIndex.SetValue( int( p_value ) );
	}

	void ModelUbo::Update( bool p_shadowReceiver
		, uint32_t p_materialIndex )const
	{
		m_shadowReceiver.SetValue( p_shadowReceiver ? 1 : 0 );
		m_materialIndex.SetValue( p_materialIndex - 1 );
		m_ubo.Update();
	}
}
