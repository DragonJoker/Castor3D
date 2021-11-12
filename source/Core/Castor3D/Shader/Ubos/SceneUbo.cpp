#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
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

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		SceneData::SceneData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_ambientLight{ getMember< sdw::Vec4 >( "ambientLight" ) }
			, m_backgroundColour{ getMember< sdw::Vec4 >( "backgroundColour" ) }
			, m_lightsCount{ getMember< sdw::Vec4 >( "lightsCount" ) }
			, m_cameraPosition{ getMember< sdw::Vec4 >( "cameraPosition" ) }
			, m_clipInfo{ getMember< sdw::Vec4 >( "clipInfo" ) }
			, m_fogInfo{ getMember< sdw::Vec4 >( "fogInfo" ) }
			, fogType{ writer.cast< sdw::UInt >( m_fogInfo.x() ) }
			, ambientLight{ m_ambientLight.xyz() }
			, cameraPosition{ m_cameraPosition.xyz() }
			, renderSize{ m_clipInfo.xy() }
			, nearPlane{ m_clipInfo.z() }
			, farPlane{ m_clipInfo.w() }
			, directionalLightCount{ writer.cast< sdw::Int >( m_lightsCount.x() ) }
			, pointLightCount{ writer.cast< sdw::Int >( m_lightsCount.y() ) }
			, spotLightCount{ writer.cast< sdw::Int >( m_lightsCount.z() ) }
		{
		}

		ast::type::BaseStructPtr SceneData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_SceneData" );

			if ( result->empty() )
			{
				result->declMember( "ambientLight", ast::type::Kind::eVec4F );
				result->declMember( "backgroundColour", ast::type::Kind::eVec4F );
				result->declMember( "lightsCount", ast::type::Kind::eVec4F );
				result->declMember( "cameraPosition", ast::type::Kind::eVec4F );
				result->declMember( "clipInfo", ast::type::Kind::eVec4F );
				result->declMember( "fogInfo", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > SceneData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec3 SceneData::transformCamera( sdw::Mat3 const & transform )const
		{
			return transform * cameraPosition;
		}

		sdw::Vec3 SceneData::getPosToCamera( sdw::Vec3 const & position )const
		{
			return cameraPosition - position;
		}

		sdw::Vec3 SceneData::getCameraToPos( sdw::Vec3 const & position )const
		{
			return position - cameraPosition;
		}

		sdw::Vec4 SceneData::getBackgroundColour( Utils const & utils
			, sdw::Float const & gamma )const
		{
			return vec4( utils.removeGamma( gamma, m_backgroundColour.rgb() ), m_backgroundColour.a() );
		}

		sdw::Vec4 SceneData::getBackgroundColour( HdrConfigData const & hdrConfigData )const
		{
			return vec4( hdrConfigData.removeGamma( m_backgroundColour.rgb() ), m_backgroundColour.a() );
		}

		sdw::Vec4 SceneData::computeAccumulation( Utils const & utils
			, sdw::Float const & depth
			, sdw::Vec3 const & colour
			, sdw::Float const & alpha
			, sdw::Float const & accumulationOperator )const
		{
			return utils.computeAccumulation( depth
				, colour
				, alpha
				, nearPlane
				, farPlane
				, accumulationOperator );
		}
	}

	//*********************************************************************************************

	castor::String const SceneUbo::BufferScene = cuT( "Scene" );
	castor::String const SceneUbo::SceneData = cuT( "c3d_sceneData" );

	SceneUbo::SceneUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}

	SceneUbo::~SceneUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
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
			configuration.clipInfo[0] = float( camera->getWidth() );
			configuration.clipInfo[1] = float( camera->getHeight() );
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
		auto & data = m_ubo.getData();
		data.clipInfo[0] = float( window[0] );
		data.clipInfo[1] = float( window[1] );
	}

	//*********************************************************************************************
}
