#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/Writer.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( c3d, LpvLightConfigUbo )

namespace c3d
{
	//*********************************************************************************************

	namespace lpvlubo
	{
		static Matrix4x4f snapMatrix( float lpvCellSize
			, Matrix4x4f mtx )
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
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( MemoryPropertyFlags::eNone ) }
	{
	}
	
	LpvLightConfigUbo::~LpvLightConfigUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void LpvLightConfigUbo::cpuUpdate( LightInstance const & light
		, float lpvCellSize
		, uint32_t faceIndex )
	{
		CU_Require( m_ubo );
		auto & lpvConfig = light.getCategory().getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.lightOffset = float( light.getBufferOffset() );
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		auto ltType = light.getCategory().getLightType();

		switch ( ltType )
		{
		case LightType::eDirectional:
			CU_Require( faceIndex == 0u );
			configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
				, static_cast< DirectionalLightInstance const & >( light ).getViewMatrix( MaxDirectionalCascadesCount - 1u ) );
			configuration.tanFovXHalf = 1.0f;
			configuration.tanFovYHalf = 1.0f;
			break;

		case LightType::ePoint:
			{
				CU_Require( faceIndex < 6u );
				Angle const lightFov = 90.0_degrees;
				configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
					, static_cast< PointLightInstance const & >( light ).getViewMatrix( CubeMapFace( faceIndex ) ) );
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;

		case LightType::eSpot:
			{
				CU_Require( faceIndex == 0u );
				configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
					, static_cast< SpotLightInstance const & >( light ).getViewMatrix() );
				Angle const & lightFov = static_cast< SpotLight const & >( light.getCategory() ).getOuterCutOff();
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;

		default:
			CU_Failure( "Unsupported LightType" );
			break;
		}
	}

	void LpvLightConfigUbo::cpuUpdate( DirectionalLightInstance const & light
		, uint32_t cascadeIndex
		, float lpvCellSize )
	{
		auto & lpvConfig = light.getCategory().getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.lightView = lpvlubo::snapMatrix( lpvCellSize
			, light.getViewMatrix( cascadeIndex ) );
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		configuration.tanFovXHalf = 1.0f;
		configuration.tanFovYHalf = 1.0f;
		configuration.lightOffset = float( light.getBufferIndex() );
	}
}
