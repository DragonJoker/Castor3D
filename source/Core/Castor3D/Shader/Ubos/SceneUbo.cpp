#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, SceneUbo )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::Vec3 SceneData::transformCamera( sdw::Mat3 const & transform )const
		{
			return transform * cameraPosition();
		}

		sdw::Vec3 SceneData::getPosToCamera( sdw::Vec3 const & position )const
		{
			return cameraPosition() - position;
		}

		sdw::Vec3 SceneData::getCameraToPos( sdw::Vec3 const & position )const
		{
			return position - cameraPosition();
		}

		sdw::Vec4 SceneData::getBackgroundColour( Utils & utils )const
		{
			return vec4( utils.removeGamma( gamma(), backgroundColour().rgb() ), backgroundColour().a() );
		}

		sdw::Vec4 SceneData::getBackgroundColour( HdrConfigData const & hdrConfigData )const
		{
			return vec4( hdrConfigData.removeGamma( backgroundColour().rgb() ), backgroundColour().a() );
		}

		sdw::Vec4 SceneData::computeAccumulation( Utils & utils
			, sdw::Float const & depth
			, sdw::Vec3 const & colour
			, sdw::Float const & alpha
			, sdw::UInt const & accumulationOperator )const
		{
			return utils.computeAccumulation( depth
				, colour
				, alpha
				, nearPlane()
				, farPlane()
				, accumulationOperator );
		}

		sdw::Vec2 SceneData::cameraPlanes()const
		{
			return vec2( nearPlane(), farPlane() );
		}
	}

	//*********************************************************************************************

	SceneUbo::SceneUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	SceneUbo::~SceneUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void SceneUbo::cpuUpdateCameraPosition( Camera const & camera )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		auto position = camera.getParent()->getDerivedPosition();
		configuration.cameraPos = castor::Point3f{ position[0], position[1], position[2] };
		configuration.gamma = camera.getHdrConfig().gamma;
		configuration.ambientLight = toRGBFloat( camera.getScene()->getAmbientLight() );
		configuration.gamma = camera.getHdrConfig().gamma;
		configuration.backgroundColour = toRGBAFloat( camera.getScene()->getBackgroundColour() );
	}

	void SceneUbo::cpuUpdate( Camera const * camera
		, Fog const & fog )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.fogType = uint32_t( fog.getType() );
		configuration.fogDensity = fog.getDensity();

		if ( camera )
		{
			configuration.nearPlane = camera->getNear();
			configuration.farPlane = camera->getFar();
			cpuUpdateCameraPosition( *camera );
			setWindowSize( { camera->getWidth(), camera->getHeight() } );
		}
	}

	void SceneUbo::cpuUpdate( Scene const & scene
		, Camera const * camera )
	{
		cpuUpdate( camera, scene.getFog() );
	}

	void SceneUbo::setWindowSize( castor::Size const & window )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		auto size = getSafeBandedSize( window );
		data.renderSize = castor::Point2f{ float( size[0] ), float( size[1] ) };
	}

	//*********************************************************************************************
}
