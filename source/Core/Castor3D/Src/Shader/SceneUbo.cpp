#include "SceneUbo.hpp"

#include "Engine.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBufferBinding.hpp"
#include "Texture/TextureLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	SceneUbo::SceneUbo( Engine & p_engine )
		: m_ubo{ ShaderProgram::BufferScene, *p_engine.GetRenderSystem() }
		, m_ambientLight{ *m_ubo.CreateUniform< UniformType::eVec4f >( ShaderProgram::AmbientLight ) }
		, m_backgroundColour{ *m_ubo.CreateUniform< UniformType::eVec4f >( ShaderProgram::BackgroundColour ) }
		, m_lightsCount{ *m_ubo.CreateUniform< UniformType::eVec4i >( ShaderProgram::LightsCount ) }
		, m_cameraPos{ *m_ubo.CreateUniform< UniformType::eVec3f >( ShaderProgram::CameraPos ) }
		, m_cameraFarPlane{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::CameraFarPlane ) }
		, m_fogType{ *m_ubo.CreateUniform< UniformType::eInt >( ShaderProgram::FogType ) }
		, m_fogDensity{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::FogDensity ) }
	{
	}

	SceneUbo::~SceneUbo()
	{
	}

	void SceneUbo::Update( Scene const & p_scene
		, Camera const & p_camera
		, bool p_lights )const
	{
		auto & l_fog = p_scene.GetFog();
		m_fogType.SetValue( int( l_fog.GetType() ) );

		if ( l_fog.GetType() != GLSL::FogType::eDisabled )
		{
			m_fogDensity.SetValue( l_fog.GetDensity() );
		}

		if ( p_lights )
		{
			auto & l_cache = p_scene.GetLightCache();
			auto l_lock = make_unique_lock( l_cache );
			m_lightsCount.GetValue( 0 )[size_t( LightType::eSpot )] = l_cache.GetLightsCount( LightType::eSpot );
			m_lightsCount.GetValue( 0 )[size_t( LightType::ePoint )] = l_cache.GetLightsCount( LightType::ePoint );
			m_lightsCount.GetValue( 0 )[size_t( LightType::eDirectional )] = l_cache.GetLightsCount( LightType::eDirectional );
		}

		m_ambientLight.SetValue( rgba_float( p_scene.GetAmbientLight() ) );
		m_backgroundColour.SetValue( rgba_float( p_scene.GetBackgroundColour() ) );
		m_cameraPos.SetValue( p_camera.GetParent()->GetDerivedPosition() );
		m_cameraFarPlane.SetValue( p_camera.GetViewport().GetFar() );
		m_ubo.Update();
	}
}
