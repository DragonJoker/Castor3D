#include "SceneUbo.hpp"

#include "Engine.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBufferBinding.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	SceneUbo::SceneUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferScene
			, *engine.getRenderSystem()
			, SceneUbo::BindingPoint }
		, m_ambientLight{ *m_ubo.createUniform< UniformType::eVec4f >( ShaderProgram::AmbientLight ) }
		, m_backgroundColour{ *m_ubo.createUniform< UniformType::eVec4f >( ShaderProgram::BackgroundColour ) }
		, m_lightsCount{ *m_ubo.createUniform< UniformType::eVec4i >( ShaderProgram::LightsCount ) }
		, m_cameraPos{ *m_ubo.createUniform< UniformType::eVec3f >( ShaderProgram::CameraPos ) }
		, m_cameraNearPlane{ *m_ubo.createUniform< UniformType::eFloat >( ShaderProgram::CameraNearPlane ) }
		, m_cameraFarPlane{ *m_ubo.createUniform< UniformType::eFloat >( ShaderProgram::CameraFarPlane ) }
		, m_fogType{ *m_ubo.createUniform< UniformType::eInt >( ShaderProgram::FogType ) }
		, m_fogDensity{ *m_ubo.createUniform< UniformType::eFloat >( ShaderProgram::FogDensity ) }
	{
	}

	SceneUbo::~SceneUbo()
	{
	}

	void SceneUbo::updateCameraPosition( Camera const & p_camera )const
	{
		m_cameraPos.setValue( p_camera.getParent()->getDerivedPosition() );
		m_ambientLight.setValue( toRGBAFloat( p_camera.getScene()->getAmbientLight() ) );
		m_backgroundColour.setValue( toRGBAFloat( p_camera.getScene()->getBackgroundColour() ) );
		m_ubo.update();
		m_ubo.bindTo( SceneUbo::BindingPoint );
	}

	void SceneUbo::update( Camera const & p_camera
		, Fog const & p_fog )const
	{
		m_fogType.setValue( int( p_fog.getType() ) );

		if ( p_fog.getType() != glsl::FogType::eDisabled )
		{
			m_fogDensity.setValue( p_fog.getDensity() );
		}

		m_cameraNearPlane.setValue( p_camera.getViewport().getNear() );
		m_cameraFarPlane.setValue( p_camera.getViewport().getFar() );
		updateCameraPosition( p_camera );
	}

	void SceneUbo::update( Scene const & p_scene
		, Camera const & p_camera
		, bool p_lights )const
	{
		if ( p_lights )
		{
			auto & cache = p_scene.getLightCache();
			auto lock = makeUniqueLock( cache );
			m_lightsCount.getValue( 0 )[size_t( LightType::eSpot )] = cache.getLightsCount( LightType::eSpot );
			m_lightsCount.getValue( 0 )[size_t( LightType::ePoint )] = cache.getLightsCount( LightType::ePoint );
			m_lightsCount.getValue( 0 )[size_t( LightType::eDirectional )] = cache.getLightsCount( LightType::eDirectional );
		}

		update( p_camera, p_scene.getFog() );
	}
}
