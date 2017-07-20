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
		, m_cameraNearPlane{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::CameraNearPlane ) }
		, m_cameraFarPlane{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::CameraFarPlane ) }
		, m_fogType{ *m_ubo.CreateUniform< UniformType::eInt >( ShaderProgram::FogType ) }
		, m_fogDensity{ *m_ubo.CreateUniform< UniformType::eFloat >( ShaderProgram::FogDensity ) }
	{
	}

	SceneUbo::~SceneUbo()
	{
	}

	void SceneUbo::UpdateCameraPosition( Camera const & p_camera )const
	{
		m_cameraPos.SetValue( p_camera.GetParent()->GetDerivedPosition() );
		m_ambientLight.SetValue( rgba_float( p_camera.GetScene()->GetAmbientLight() ) );
		m_backgroundColour.SetValue( rgba_float( p_camera.GetScene()->GetBackgroundColour() ) );
		m_ubo.Update();
		m_ubo.BindTo( SceneUbo::BindingPoint );
	}

	void SceneUbo::Update( Camera const & p_camera
		, Fog const & p_fog )const
	{
		m_fogType.SetValue( int( p_fog.GetType() ) );

		if ( p_fog.GetType() != GLSL::FogType::eDisabled )
		{
			m_fogDensity.SetValue( p_fog.GetDensity() );
		}

		m_cameraNearPlane.SetValue( p_camera.GetViewport().GetNear() );
		m_cameraFarPlane.SetValue( p_camera.GetViewport().GetFar() );
		UpdateCameraPosition( p_camera );
	}

	void SceneUbo::Update( Scene const & p_scene
		, Camera const & p_camera
		, bool p_lights )const
	{
		if ( p_lights )
		{
			auto & cache = p_scene.GetLightCache();
			auto lock = make_unique_lock( cache );
			m_lightsCount.GetValue( 0 )[size_t( LightType::eSpot )] = cache.GetLightsCount( LightType::eSpot );
			m_lightsCount.GetValue( 0 )[size_t( LightType::ePoint )] = cache.GetLightsCount( LightType::ePoint );
			m_lightsCount.GetValue( 0 )[size_t( LightType::eDirectional )] = cache.GetLightsCount( LightType::eDirectional );
		}

		Update( p_camera, p_scene.GetFog() );
	}
}
