#include "ModelUbo.hpp"

#include "Engine.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	ModelUbo::ModelUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferModel
			, *engine.getRenderSystem()
			, ModelUbo::BindingPoint }
		, m_shadowReceiver{ *m_ubo.createUniform< UniformType::eInt >( ShaderProgram::ShadowReceiver ) }
		, m_materialIndex{ *m_ubo.createUniform< UniformType::eInt >( ShaderProgram::MaterialIndex ) }
		, m_environmentIndex{ *m_ubo.createUniform< UniformType::eInt >( ShaderProgram::EnvironmentIndex ) }
	{
	}

	ModelUbo::~ModelUbo()
	{
	}

	void ModelUbo::setEnvMapIndex( uint32_t p_value )
	{
		m_environmentIndex.setValue( int( p_value ) );
	}

	void ModelUbo::update( bool p_shadowReceiver
		, uint32_t p_materialIndex )const
	{
		m_shadowReceiver.setValue( p_shadowReceiver ? 1 : 0 );
		m_materialIndex.setValue( p_materialIndex - 1 );
		m_ubo.update();
		m_ubo.bindTo( ModelUbo::BindingPoint );
	}
}
