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
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, SceneUbo )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::Vec4 SceneData::getBackgroundColour( Utils & utils
			, sdw::Float const gamma )const
		{
			return vec4( utils.removeGamma( gamma, backgroundColour() ), 1.0_f );
		}

		sdw::Vec4 SceneData::getBackgroundColour( HdrConfigData const & hdrConfigData )const
		{
			return vec4( hdrConfigData.removeGamma( backgroundColour() ), 1.0_f );
		}

		sdw::Vec4 SceneData::computeAccumulation( Utils & utils
			, CameraData const & camera
			, sdw::Float const & depth
			, sdw::Vec3 const & colour
			, sdw::Float const & alpha
			, sdw::UInt const & accumulationOperator )const
		{
			return utils.computeAccumulation( depth
				, colour
				, alpha
				, camera.nearPlane()
				, camera.farPlane()
				, accumulationOperator );
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

	SceneUbo::Configuration & SceneUbo::cpuUpdate( Fog const & fog )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.fogType = uint32_t( fog.getType() );
		configuration.fogDensity = fog.getDensity();
		return configuration;
	}

	SceneUbo::Configuration & SceneUbo::cpuUpdate( Scene const & scene )
	{
		auto & configuration = cpuUpdate( scene.getFog() );
		configuration.ambientLight = toRGBFloat( scene.getAmbientLight() );
		configuration.backgroundColour = toRGBFloat( scene.getBackgroundColour() );
		return configuration;
	}

	//*********************************************************************************************
}
