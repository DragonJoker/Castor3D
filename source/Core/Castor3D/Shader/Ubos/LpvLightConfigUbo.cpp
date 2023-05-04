#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/Writer.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, LpvLightConfigUbo )

namespace castor3d
{
	//*********************************************************************************************

	namespace lpvlubo
	{
		static castor::Matrix4x4f snapMatrix( float lpvCellSize
			, castor::Matrix4x4f mtx )
		{
			mtx[0][3] = float( mtx[0][3] - fmod( mtx[0][3], lpvCellSize ) );
			mtx[1][3] = float( mtx[1][3] - fmod( mtx[1][3], lpvCellSize ) );
			mtx[2][3] = float( mtx[2][3] - fmod( mtx[2][3], lpvCellSize ) );
			return mtx;
		}
	}

	//*********************************************************************************************

	LpvLightConfigUbo::LpvLightConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}
	
	LpvLightConfigUbo::~LpvLightConfigUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void LpvLightConfigUbo::cpuUpdate( Light const & light
		, float lpvCellSize
		, uint32_t faceIndex )
	{
		CU_Require( m_ubo );
		auto & lpvConfig = light.getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.lightOffset = float( light.getBufferOffset() );
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		auto ltType = light.getLightType();

		switch ( ltType )
		{
		case LightType::eDirectional:
			CU_Require( faceIndex == 0u );
			configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
				, light.getDirectionalLight()->getViewMatrix( MaxDirectionalCascadesCount - 1u ) );
			configuration.tanFovXHalf = 1.0f;
			configuration.tanFovYHalf = 1.0f;
			break;

		case LightType::ePoint:
			{
				CU_Require( faceIndex < 6u );
				auto lightFov = 90.0_degrees;
				auto & pointLight = *light.getPointLight();
				configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
					, pointLight.getViewMatrix( CubeMapFace( faceIndex ) ) );
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;

		case LightType::eSpot:
			{
				CU_Require( faceIndex == 0u );
				auto & spotLight = *light.getSpotLight();
				configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
					, spotLight.getViewMatrix() );
				auto lightFov = spotLight.getOuterCutOff();
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;

		default:
			CU_Failure( "Unsupported LightType" );
			break;
		}
	}

	void LpvLightConfigUbo::cpuUpdate( DirectionalLight const & light
		, uint32_t cascadeIndex
		, float lpvCellSize )
	{
		auto & lpvConfig = light.getLight().getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
			, light.getViewMatrix( cascadeIndex ) );
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		configuration.tanFovXHalf = 1.0f;
		configuration.tanFovYHalf = 1.0f;
		configuration.lightOffset = float( light.getLight().getBufferIndex() );
	}
}
