#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	uint32_t const SceneUbo::BindingPoint = 4u;
	String const SceneUbo::BufferScene = cuT( "Scene" );
	String const SceneUbo::AmbientLight = cuT( "c3d_ambientLight" );
	String const SceneUbo::LightsCount = cuT( "c3d_lightsCount" );
	String const SceneUbo::BackgroundColour = cuT( "c3d_backgroundColour" );
	String const SceneUbo::CameraPos = cuT( "c3d_cameraPosition" );
	String const SceneUbo::ClipInfo = cuT( "c3d_clipInfo" );
	String const SceneUbo::FogInfo = cuT( "c3d_fogInfo" );

	SceneUbo::SceneUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->getCurrentRenderDevice() )
		{
			initialise();
		}
	}

	SceneUbo::~SceneUbo()
	{
	}

	void SceneUbo::initialise()
	{
		if ( !m_ubo )
		{
			auto & device = getCurrentRenderDevice( m_engine );
			m_ubo = makeUniformBuffer< Configuration >( device
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "SceneUbo" );
		}
	}

	void SceneUbo::cleanup()
	{
		m_ubo.reset();
	}

	void SceneUbo::updateCameraPosition( Camera const & camera )const
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		auto position = camera.getParent()->getDerivedPosition();
		configuration.cameraPos = Point4f{ position[0], position[1], position[2], 0.0 };
		configuration.ambientLight = toRGBAFloat( camera.getScene()->getAmbientLight() );
		configuration.backgroundColour = toRGBAFloat( camera.getScene()->getBackgroundColour() );
		m_ubo->upload();
	}

	void SceneUbo::update( Camera const * camera
		, Fog const & fog )const
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.fogInfo[0] = float( fog.getType() );
		configuration.fogInfo[1] = fog.getDensity();

		if ( camera )
		{
			configuration.clipInfo[2] = camera->getNear();
			configuration.clipInfo[3] = camera->getFar();
			updateCameraPosition( *camera );
		}
		else
		{
			m_ubo->upload();
		}
	}

	void SceneUbo::update( Scene const & scene
		, Camera const * camera
		, bool lights )const
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );

		if ( lights )
		{
			auto & cache = scene.getLightCache();
			auto lock = makeUniqueLock( cache );
			configuration.lightsCount[size_t( LightType::eSpot )] = float( cache.getLightsCount( LightType::eSpot ) );
			configuration.lightsCount[size_t( LightType::ePoint )] = float( cache.getLightsCount( LightType::ePoint ) );
			configuration.lightsCount[size_t( LightType::eDirectional )] = float( cache.getLightsCount( LightType::eDirectional ) );
		}

		update( camera, scene.getFog() );
	}

	void SceneUbo::setWindowSize( Size const & window )const
	{
		CU_Require( m_ubo );
		m_ubo->getData( 0u ).clipInfo[0] = float( window[0] );
		m_ubo->getData( 0u ).clipInfo[1] = float( window[1] );
	}
}
