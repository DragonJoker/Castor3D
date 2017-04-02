#include "SceneRenderNode.hpp"

#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"

namespace Castor3D
{
	SceneRenderNode::SceneRenderNode( UniformBuffer & p_sceneUbo )
		: m_sceneUbo{ p_sceneUbo }
		, m_ambientLight{ *p_sceneUbo.GetUniform< UniformType::eVec4f >( ShaderProgram::AmbientLight ) }
		, m_backgroundColour{ *p_sceneUbo.GetUniform< UniformType::eVec4f >( ShaderProgram::BackgroundColour ) }
		, m_lightsCount{ *p_sceneUbo.GetUniform< UniformType::eVec4i >( ShaderProgram::LightsCount ) }
		, m_cameraPos{ *p_sceneUbo.GetUniform< UniformType::eVec3f >( ShaderProgram::CameraPos ) }
		, m_cameraFarPlane{ *p_sceneUbo.GetUniform< UniformType::eFloat >( ShaderProgram::CameraFarPlane ) }
		, m_fogType{ *p_sceneUbo.GetUniform< UniformType::eInt >( ShaderProgram::FogType ) }
		, m_fogDensity{ *p_sceneUbo.GetUniform< UniformType::eFloat >( ShaderProgram::FogDensity ) }
	{
	}
}
