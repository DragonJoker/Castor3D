#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	uint32_t const SceneUbo::BindingPoint = 4u;
	castor::String const SceneUbo::BufferScene = cuT( "Scene" );
	castor::String const SceneUbo::AmbientLight = cuT( "c3d_ambientLight" );
	castor::String const SceneUbo::LightsCount = cuT( "c3d_lightsCount" );
	castor::String const SceneUbo::BackgroundColour = cuT( "c3d_backgroundColour" );
	castor::String const SceneUbo::CameraPos = cuT( "c3d_cameraPosition" );
	castor::String const SceneUbo::ClipInfo = cuT( "c3d_clipInfo" );
	castor::String const SceneUbo::FogInfo = cuT( "c3d_fogInfo" );

	SceneUbo::SceneUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise( engine.getRenderSystem()->getCurrentRenderDevice() );
		}
	}
	
	SceneUbo::SceneUbo( RenderDevice const & device )
		: m_engine{ *device.renderSystem.getEngine() }
	{
		initialise( device );
	}

	SceneUbo::~SceneUbo()
	{
		cleanup();
	}

	void SceneUbo::initialise( RenderDevice const & device )
	{
		if ( !m_ubo )
		{
			m_device = &device;
			m_ubo = device.uboPools->getBuffer< Configuration >( 0u );
		}
	}

	void SceneUbo::cleanup()
	{
		if ( m_ubo && m_device )
		{
			m_device->uboPools->putBuffer( m_ubo );
			m_device = nullptr;
			m_ubo = {};
		}
	}

	void SceneUbo::cpuUpdateCameraPosition( Camera const & camera )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		auto position = camera.getParent()->getDerivedPosition();
		configuration.cameraPos = castor::Point4f{ position[0], position[1], position[2], 0.0 };
		configuration.ambientLight = toRGBAFloat( camera.getScene()->getAmbientLight() );
		configuration.backgroundColour = toRGBAFloat( camera.getScene()->getBackgroundColour() );
	}

	void SceneUbo::cpuUpdate( Camera const * camera
		, Fog const & fog )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.fogInfo[0] = float( fog.getType() );
		configuration.fogInfo[1] = fog.getDensity();

		if ( camera )
		{
			configuration.clipInfo[2] = camera->getNear();
			configuration.clipInfo[3] = camera->getFar();
			cpuUpdateCameraPosition( *camera );
		}
	}

	void SceneUbo::cpuUpdate( Scene const & scene
		, Camera const * camera
		, bool lights )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();

		if ( lights )
		{
			auto & cache = scene.getLightCache();
			using LockType = std::unique_lock< LightCache const >;
			LockType lock{ castor::makeUniqueLock( cache ) };
			configuration.lightsCount[size_t( LightType::eSpot )] = float( cache.getLightsCount( LightType::eSpot ) );
			configuration.lightsCount[size_t( LightType::ePoint )] = float( cache.getLightsCount( LightType::ePoint ) );
			configuration.lightsCount[size_t( LightType::eDirectional )] = float( cache.getLightsCount( LightType::eDirectional ) );
		}

		cpuUpdate( camera, scene.getFog() );
	}

	void SceneUbo::setWindowSize( castor::Size const & window )
	{
		CU_Require( m_ubo );
		m_ubo.getData().clipInfo[0] = float( window[0] );
		m_ubo.getData().clipInfo[1] = float( window[1] );
	}
}
